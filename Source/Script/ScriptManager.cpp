#include "ScriptManager.hpp"

#include <nowide/fstream.hpp>

#include <angelscript.h>

#include <scripthelper/scripthelper.h>
#include <serializer/serializer.h>

using namespace Script;

namespace
{
    class BytecodeStore : public asIBinaryStream
    {
    public:
        void Read(void *ptr, asUINT size)
        {
            char* data = (char*)ptr;

            for (uint32_t i = 0; i < size; ++i)
            {
                data[i] = mStore[i];
            }
        }
        void Write(const void *ptr, asUINT size)
        {
            const char* data = (const char*)ptr;

            for (uint32_t i = 0; i < size; ++i)
                mStore.push_back(data[i]);
        }

    private:
        std::vector<char> mStore;
    };
}

struct ScriptManager::CoRoutine
{
    Util::Timestamp SleepUntil;
    asIScriptContext* Context;
};

ScriptManager::ScriptManager(asIScriptEngine* eng) :
    mEngine(eng), mMetaMod(mEngine->GetModule("MetaMod", asGM_ALWAYS_CREATE))
{
    
}
ScriptManager::~ScriptManager()
{
    for (auto& it : mCoRoutines)
        delete it;
}

bool ScriptManager::loadScriptFromFile(const std::string& file)
{
    nowide::ifstream ifs;
    ifs.open(file.c_str(), std::ios::in | std::ios::binary);

    ifs.seekg(0, std::ios::end);
    size_t len = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<char> storage(len);
    ifs.read(&storage[0], len);

    return loadScriptFromMemory(file, &storage[0], len);
}
bool ScriptManager::loadScriptFromMemory(const std::string& file, const std::string& data)
{
    return loadScriptFromMemory(file, data.c_str(), data.size());
}
bool ScriptManager::loadScriptFromMemory(const std::string& file, const char* data, size_t length)
{
    auto it = std::find(mLoadedScripts.begin(), mLoadedScripts.end(), file);
    bool reload = it != mLoadedScripts.end();

    if (reload)
    {
        mBuilder.StartNewModule(mEngine, "!!Scratchspace!!");
        mBuilder.AddSectionFromMemory(file.c_str(), data, length);

        int ret = mBuilder.BuildModule();
        if (ret < 0)
            return false;

        CSerializer serial;
        for (auto& type : mSerializerTypes)
        {
            serial.AddUserType(std::get<1>(type)(), std::get<0>(type));
        }

        auto module = mEngine->GetModule(file.c_str());
        std::vector<asIScriptObject*> stored;
        for (auto& obj : mObjects)
        {
            if (obj->GetObjectType()->GetModule() == module)
            {
                serial.AddExtraObjectToStore(obj);
                stored.push_back(obj);
            }
        }

        serial.Store(module);

        {
            BytecodeStore store;
            auto* mod = mBuilder.GetModule();
            mod->SaveByteCode(&store);
            mod->Discard();

            module->LoadByteCode(&store);
        }

        serial.Restore(module);

        for (auto& obj : stored)
        {
            notifyObjectRemoved(obj);

            auto* newObj = reinterpret_cast<asIScriptObject*>(serial.GetPointerToRestoredObject(obj));

            notifyNewObject(newObj);

            ///\TODO Notify the object of the change
        }

        return true;
    }
    else
    {
        mBuilder.StartNewModule(mEngine, file.c_str());
        mBuilder.AddSectionFromMemory(file.c_str(), data, length);

        int ret = mBuilder.BuildModule();
        if (ret < 0)
            return false;

        auto ctx = mEngine->RequestContext();
        auto mod = mBuilder.GetModule();
        mod->SetUserData(this);

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

        mLoadedScripts.push_back(file);
        return true;
    }
}

void ScriptManager::runCoroutines()
{
    Util::Timestamp now = Util::ClockImpl::now();

    for (auto it = mCoRoutines.begin(); it != mCoRoutines.end();)
    {
        auto* co = *it;

        if (co->SleepUntil > now)
            continue;

        uint32_t gcSize1, gcSize2;
        mEngine->GetGCStatistics(&gcSize1);

        ///\TODO Add a timeout for the co-routine execution
        int r = co->Context->Execute();

        mEngine->GetGCStatistics(&gcSize2);

        if (gcSize2 > gcSize1)
            mEngine->GarbageCollect(asGC_FULL_CYCLE | asGC_DESTROY_GARBAGE);

        mEngine->GarbageCollect(asGC_ONE_STEP | asGC_DETECT_GARBAGE);

        // Execution of the co-routine ended for some reason
        if (r != asEXECUTION_SUSPENDED)
        {
            mEngine->ReturnContext(co->Context);
            delete co;

            mCoRoutines.erase(it++);
            continue;
        }
        
        ++it;
    }
}

void ScriptManager::defineWord(const std::string& word)
{
    mBuilder.DefineWord(word.c_str());
}

void ScriptManager::notifyNewObject(asIScriptObject* obj)
{
    mObjects.push_back(obj);
}
void ScriptManager::notifyObjectRemoved(asIScriptObject* obj)
{
    auto it = std::find(mObjects.begin(), mObjects.end(), obj);
    if (it != mObjects.end())
        mObjects.erase(it);
}

void ScriptManager::registerType(const std::string& name, const SerializerCallback_t& type)
{
    mSerializerTypes.push_back(std::make_tuple(name, type));
}

asIScriptContext* ScriptManager::addCoRoutine(asIScriptFunction* func)
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

void ScriptManager::sleep(asIScriptContext* ctx, const Util::Timespan& duration)
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

void ScriptManager::runMetadata(const std::string& meta, asIScriptContext* ctx)
{
    auto mod = mBuilder.GetModule();

    size_t cur = 0;
    do
    {
        size_t next = meta.find(';', cur);
        ExecuteString(mEngine, ("Meta::" + meta.substr(cur, next)).c_str(), mod, ctx);

        if (next == std::string::npos)
            break;

        cur = next + 1;
    } while (true);
}