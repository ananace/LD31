#include "ScriptManager.hpp"
#include "ScriptExtensions.hpp"
#include "ScriptHooks.hpp"
#include "ScriptObject.hpp"
#include <Util/FileSystem.hpp>

#include <nowide/fstream.hpp>

#include <angelscript.h>

#include <scriptarray/scriptarray.h>
#include <scripthelper/scripthelper.h>
#include <serializer_fix/serializer.h>

using namespace Script;

Manager Script::ScriptManager;

namespace
{
    // This serializes the std::string type
    struct CStringType : public CUserType
    {
        void Store(CSerializedValue *val, void *ptr)
        {
            val->SetUserData(new std::string(*(std::string*)ptr));
        }
        void Restore(CSerializedValue *val, void *ptr)
        {
            std::string *buffer = (std::string*)val->GetUserData();
            *(std::string*)ptr = *buffer;
        }
        void CleanupUserData(CSerializedValue *val)
        {
            std::string *buffer = (std::string*)val->GetUserData();
            delete buffer;
        }
    };
    // This serializes the CScriptArray type
    struct CArrayType : public CUserType
    {
        void Store(CSerializedValue *val, void *ptr)
        {
            CScriptArray *arr = (CScriptArray*)ptr;
            for (unsigned int i = 0; i < arr->GetSize(); i++)
                val->m_children.push_back(new CSerializedValue(val, "", "", arr->At(i), arr->GetElementTypeId()));
        }
        void Restore(CSerializedValue *val, void *ptr)
        {
            CScriptArray *arr = (CScriptArray*)ptr;
            arr->Resize(val->m_children.size());
            for (size_t i = 0; i < val->m_children.size(); ++i)
                val->m_children[i]->Restore(arr->At(i), arr->GetElementTypeId());
        }
    };

    class BytecodeStore : public asIBinaryStream
    {
    public:
        BytecodeStore() : mTellg(0) { }

        void Read(void *ptr, asUINT size)
        {
            char* data = (char*)ptr;

            for (uint32_t i = 0; i < size; ++i)
            {
                data[i] = mStore[mTellg + i];
            }

            mTellg += size;
        }
        void Write(const void *ptr, asUINT size)
        {
            const char* data = (const char*)ptr;

            for (uint32_t i = 0; i < size; ++i)
                mStore.push_back(data[i]);
        }

    private:
        std::vector<char> mStore;
        size_t mTellg;
    };

    void addCoRoutine(asIScriptFunction* func)
    {
        auto* ctx = asGetActiveContext();

        ScriptManager.addCoRoutine(func);
    }

    void sleepCoRoutine(uint32_t ms)
    {
        ScriptManager.sleep(asGetActiveContext(), std::chrono::milliseconds(ms));
    }

    void contextCallback(asIScriptContext *ctx, Util::Timestamp *timeOut)
    {
        if (Util::ClockImpl::now() > *timeOut)
            ctx->Suspend();
    }


    bool Reg()
    {
        ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            ScriptManager.setEngine(eng);
            int r = 0;

            r = eng->RegisterFuncdef("void COROUTINE()"); assert(r >= 0);
            r = eng->RegisterGlobalFunction("void CreateCoRoutine(COROUTINE@+)", asFUNCTION(addCoRoutine), asCALL_CDECL); assert(r >= 0);
            r = eng->RegisterGlobalFunction("void Sleep(float)", asFUNCTION(sleepCoRoutine), asCALL_CDECL); assert(r >= 0);
        });
        return true;
    }

    void skipWhitespace(std::string::iterator& it, const std::string::iterator& end)
    {
        char c = *it;
        if (c != ' ' && c != '\t')
            return;

        for (; it != end && (c = *it) != 0 && (c == ' ' || c == '\t'); ++it);
    }

    std::string stripMetadataLines(const std::string& data)
    {
        std::istringstream iss(data);
        std::ostringstream oss;

        while (iss && !iss.eof())
        {
            std::string line;
            std::getline(iss, line);
            
            if (line.empty())
            {
                oss << std::endl;
                continue;
            }

            auto it = line.begin();
            skipWhitespace(it, line.end());

            if (it != line.end() && *it == '[')
            {
                line.insert(it, { '/', '/', ' ' });
            }

            oss << line << std::endl;
        }

        return oss.str();
    }

    int includeCallback(const char *include, const char *from, CScriptBuilder *builder, void *)
    {
        std::string path = Util::FileSystem::getDirname(Util::FileSystem::fixPath(from)) + Util::FileSystem::SEPARATOR + Util::FileSystem::fixPath(include);
        std::string scriptContent;

        {
            nowide::ifstream ifs;
            ifs.open(path.c_str());

            if (ifs.is_open() && ifs)
            {
                ifs.seekg(0, std::ios::end);
                size_t len = (size_t)ifs.tellg();
                ifs.seekg(0, std::ios::beg);

                if (len == 0)
                    return asERROR;

                std::vector<char> storage(len);
                ifs.read(&storage[0], len);
                scriptContent = std::string(&storage[0], len);
            }
        }

        if (scriptContent.empty())
            return asERROR;
        
        scriptContent = stripMetadataLines(scriptContent);
        ScriptManager.notifyIncluded(path, Util::FileSystem::fixPath(from));

        return builder->AddSectionFromMemory(path.c_str(), scriptContent.c_str());
    }

    uint32_t META_MASK = 0xBEEF;
}

bool ScriptExtensions::ScriptManagerExtensions = Reg();

struct Manager::CoRoutine
{
    Util::Timestamp SleepUntil;
    asIScriptContext* Context;
};

Manager::Manager() :
    mEngine(nullptr), mMetaMod(nullptr)
{
    mBuilder.SetIncludeCallback(includeCallback, nullptr);

    mSerializerTypes.push_back(std::make_tuple("string", SerializerCallback_t([]() -> CUserType* {
        return new CStringType();
    })));
    mSerializerTypes.push_back(std::make_tuple("array", SerializerCallback_t([]() -> CUserType* {
        return new CArrayType();
    })));
}
Manager::~Manager()
{
    for (auto& it : mCoRoutines)
        delete it;
}

void Manager::setEngine(asIScriptEngine* eng)
{
    for (auto& it : mCoRoutines)
        delete it;

    mLoadedScripts.clear();
    mCoRoutines.clear();
    mObjects.clear();

    mEngine = eng;
    mMetaMod = mEngine->GetModule("MetaMod", asGM_ALWAYS_CREATE);
}

bool Manager::loadScriptFromFile(const std::string& filename)
{
    std::string file = Util::FileSystem::fixPath(filename);

    nowide::ifstream ifs;
    ifs.open(file.c_str(), std::ios::binary);
    if (!ifs.is_open() || !ifs)
        return false;

    ifs.seekg(0, std::ios::end);
    size_t len = (size_t)ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<char> storage(len + 1);
    ifs.read(&storage[0], len);
    storage[len] = 0;

    return loadScriptFromMemory(file, &storage[0], len);
}
bool Manager::loadScriptFromMemory(const std::string& file, const std::string& data)
{
    return loadScriptFromMemory(file, data.c_str(), data.size());
}
bool Manager::loadScriptFromMemory(const std::string& file, const char* data, size_t length)
{
    bool reload = mLoadedScripts.count(file) > 0 && mLoadedScripts.at(file).Loaded;

    asIScriptModule* mod = nullptr;

    CSerializer serial;
    std::vector<asIScriptObject*> stored;

    if (reload)
    {
        std::cout << "Reloading " << file << "..." << std::endl;
        auto module = mEngine->GetModule(file.c_str());

        for (auto& obj : mObjects)
        {
            if (obj->GetObjectType()->GetModule() == module)
            {
                serial.AddExtraObjectToStore(obj);
                stored.push_back(obj);
            }
        }

        for (auto& type : mSerializerTypes)
        {
            serial.AddUserType(std::get<1>(type)(), std::get<0>(type));
        }

        serial.Store(module);

        // Have to build twice, a failed build breaks the script module
        mBuilder.StartNewModule(mEngine, "!!ScratchSpace!!");
        mBuilder.AddSectionFromMemory(file.c_str(), data, length);

        int ret = mBuilder.BuildModule();
        if (ret < 0)
            return false;

        mBuilder.GetModule()->Discard();

        for (uint32_t i = 0; i < module->GetObjectTypeCount(); ++i)
        {
            auto* type = module->GetObjectTypeByIndex(i);
            Script::ScriptHooks::cleanOldPossible(type);
        }

        // Perform the real build

        mBuilder.StartNewModule(mEngine, file.c_str());
        mBuilder.AddSectionFromMemory(file.c_str(), data, length);

        ret = mBuilder.BuildModule();
        assert(ret >= 0);
        
        module = mBuilder.GetModule();

        serial.Restore(module);

        mod = module;
    }
    else
    {
        mBuilder.StartNewModule(mEngine, file.c_str());
        mBuilder.AddSectionFromMemory(file.c_str(), data, length);

        int ret = mBuilder.BuildModule();
        if (ret < 0)
            return false;

        mod = mBuilder.GetModule();

        mLoadedScripts[file].Loaded = true;
    }

    auto ctx = mEngine->RequestContext();
    auto mask = mod->SetAccessMask(META_MASK);

    for (uint32_t i = 0; i < mod->GetObjectTypeCount(); ++i)
    {
        auto type = mod->GetObjectTypeByIndex(i);
        int typeId = type->GetTypeId();

        std::string data;
        if (!(data = mBuilder.GetMetadataStringForType(typeId)).empty())
        {
            mMetaMod->SetUserData(type);

            runMetadata(data, ctx);
        }

        for (uint32_t j = 0; j < type->GetMethodCount(); ++j)
        {
            auto func = type->GetMethodByIndex(j);

            if (!(data = mBuilder.GetMetadataStringForTypeMethod(typeId, func)).empty())
            {
                mMetaMod->SetUserData(func);

                runMetadata(data, ctx);
            }
        }

        for (uint32_t j = 0; j < type->GetPropertyCount(); ++j)
        {
            if (!(data = mBuilder.GetMetadataStringForTypeProperty(typeId, j)).empty())
            {
                mMetaMod->SetUserData(type);
                mMetaMod->SetUserData(&j, 1);

                runMetadata(data, ctx);
            }
        }
    }

    mod->SetAccessMask(mask);

    if (reload)
    {
        for (auto& obj : stored)
        {
            notifyObjectRemoved(obj);

            auto* newObj = reinterpret_cast<asIScriptObject*>(serial.GetPointerToRestoredObject(obj));

            ScriptObject* sObj = reinterpret_cast<ScriptObject*>(obj->GetObjectType()->GetUserData((uintptr_t)obj));
            sObj->updateObject(newObj);

            notifyNewObject(newObj);
        }

        mEngine->GarbageCollect(asGC_FULL_CYCLE | asGC_DESTROY_GARBAGE);
    }

    return true;
}

void Manager::checkForModification()
{
    auto now = Util::ClockImpl::now();
    if (now < mLastModificationCheck + std::chrono::seconds(1))
        return;

    for (auto& file : mLoadedScripts)
    {
        Util::Timestamp lastModified = Util::FileSystem::getLastModified(file.first);

        if (file.second.LastModified == Util::Timestamp())
            file.second.LastModified = lastModified;
        else if (file.second.LastModified != lastModified)
        {
            for (auto& loadedFrom : file.second.IncludedFrom)
                if (mLoadedScripts[loadedFrom].Loaded)
                    loadScriptFromFile(loadedFrom);

            if (file.second.Loaded)
                loadScriptFromFile(file.first);

            file.second.LastModified = lastModified;
        }
    }

    mLastModificationCheck = now;
}

void Manager::runCoroutines(const Util::Timespan& duration)
{
    Util::Timestamp now = Util::ClockImpl::now();
    Util::Timestamp end = now + duration; // Keep this as a total duration, or per-coroutine?

    for (auto it = mCoRoutines.begin(); it != mCoRoutines.end();)
    {
        auto* co = *it;

        if (co->SleepUntil > now)
            continue;

        uint32_t gcSize1, gcSize2;
        mEngine->GetGCStatistics(&gcSize1);

        co->Context->SetLineCallback(asFUNCTION(contextCallback), &end, asCALL_CDECL);
        int r = co->Context->Execute();

        mEngine->GetGCStatistics(&gcSize2);

        if (gcSize2 > gcSize1)
            mEngine->GarbageCollect(asGC_FULL_CYCLE | asGC_DESTROY_GARBAGE);

        mEngine->GarbageCollect(asGC_ONE_STEP | asGC_DETECT_GARBAGE);

        // Execution of the co-routine ended for some reason
        if (r != asEXECUTION_SUSPENDED)
        {
            std::ostringstream oss;
            oss << "CoRoutine for " << co->Context->GetFunction()->GetScriptSectionName() << " (" << co->Context->GetFunction()->GetDeclaration() << ") ending...";
            mEngine->WriteMessage("CoRoutines", 0, 0, asMSGTYPE_INFORMATION, oss.str().c_str());

            mEngine->ReturnContext(co->Context);
            delete co;

            mCoRoutines.erase(it++);
            continue;
        }
        
        ++it;
    }
}

void Manager::defineWord(const std::string& word)
{
    mBuilder.DefineWord(word.c_str());
}

void Manager::notifyIncluded(const std::string& script, const std::string& from)
{
    mLoadedScripts[script].IncludedFrom.insert(from);
}

void Manager::notifyNewObject(asIScriptObject* obj)
{
    mObjects.push_back(obj);
}
void Manager::notifyObjectRemoved(asIScriptObject* obj)
{
    auto it = std::find(mObjects.begin(), mObjects.end(), obj);
    if (it != mObjects.end())
        mObjects.erase(it);
}

void Manager::registerType(const std::string& name, const SerializerCallback_t& type)
{
    mSerializerTypes.push_back(std::make_tuple(name, type));
}

asIScriptContext* Manager::addCoRoutine(asIScriptFunction* func)
{
    CoRoutine* co = new CoRoutine{ Util::ClockImpl::now(), mEngine->RequestContext() };
    int r = co->Context->Prepare(func);
    if (r < 0)
    {
        mEngine->ReturnContext(co->Context);
        delete co;

        return nullptr;
    }

    mCoRoutines.push_back(co);

    return co->Context;
}

void Manager::sleep(asIScriptContext* ctx, const Util::Timespan& duration)
{
    auto until = Util::ClockImpl::now() + duration;

    std::find_if(mCoRoutines.begin(), mCoRoutines.end(), [ctx, &until](CoRoutine* co) {
        if (co->Context == ctx)
        {
            co->SleepUntil = until;

            if (co->Context->GetState() == asEXECUTION_ACTIVE)
                co->Context->Suspend();

            return true;
        }
        return false;
    });
}

void Manager::runMetadata(const std::string& meta, asIScriptContext* ctx)
{
    auto mod = mBuilder.GetModule();

    size_t cur = 0;
    do
    {
        size_t next = meta.find(';', cur);
        ExecuteString(mEngine, (meta.substr(cur, next)).c_str(), mod, ctx);

        if (next == std::string::npos)
            break;

        cur = next + 1;
    } while (true);
}

Manager::LoadedScriptData::LoadedScriptData() :
    LastModified(), Loaded(false)
{

}
