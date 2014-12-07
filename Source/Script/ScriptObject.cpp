#include "ScriptObject.hpp"
#include "ScriptExtensions.hpp"
#include "ScriptHooks.hpp"
#include "ScriptManager.hpp"

#include <angelscript.h>

#include <scriptdictionary/scriptdictionary.h>

#include <cassert>

using namespace Script;

namespace
{
    std::unordered_map<std::string, asIObjectType*> RegTypes;

    void objectType(const std::string& name)
    {
        auto* ctx = asGetActiveContext();
        auto* mod = ctx->GetEngine()->GetModule("MetaMod");

        auto* type = reinterpret_cast<asIObjectType*>(mod->GetUserData());

        RegTypes[name] = type;
    }
}

ScriptObject::ScriptObject(asIObjectType* type)
{
    auto* eng = type->GetEngine();

    mObject = reinterpret_cast<asIScriptObject*>(eng->CreateScriptObject(type));
    type->SetUserData(this, (uintptr_t)mObject);

    ScriptHooks::bindObject(mObject);
    ScriptManager.notifyNewObject(mObject);
}

ScriptObject::~ScriptObject()
{
    ScriptManager.notifyObjectRemoved(mObject);
    ScriptHooks::unbindObject(mObject);

    mObject->GetObjectType()->SetUserData(nullptr, (uintptr_t)mObject);
    mObject->Release();
}

asIScriptObject* ScriptObject::getObject()
{
    return mObject;
}

void ScriptObject::updateObject(asIScriptObject* newObject)
{
    CScriptDictionary* dict = CScriptDictionary::Create(mObject->GetEngine());

    auto* ctx = mObject->GetEngine()->RequestContext();
    auto* func = mObject->GetObjectType()->GetMethodByDecl("void PreReload(dictionary@)");
    if (func)
    {
        ctx->Prepare(func);
        ctx->SetObject(mObject);
        ctx->SetArgObject(0, dict);
        
        int r = ctx->Execute();

        ctx->Unprepare();
    }

    auto* oldType = mObject->GetObjectType();
    auto* oldObj = mObject;

    ScriptHooks::unbindObject(mObject);

    mObject->GetObjectType()->SetUserData(nullptr, (uintptr_t)mObject);

    mObject = newObject;
    if (mObject)
    {
        mObject->GetObjectType()->SetUserData(this, (uintptr_t)mObject);

        ScriptHooks::bindObject(mObject);

        func = mObject->GetObjectType()->GetMethodByDecl("void PostReload(dictionary@)");
        if (func)
        {
            ctx->Prepare(func);
            ctx->SetObject(mObject);
            ctx->SetArgObject(0, dict);

            int r = ctx->Execute();
        }
    }

    oldObj->GetEngine()->ReturnContext(ctx);
    oldObj->Release();
    dict->Release();

    if (!mObject)
        delete this;
}

ScriptObject* ScriptObject::Create(const std::string& name)
{
    if (RegTypes.count(name) == 0)
        return nullptr;

    return new ScriptObject(RegTypes[name]);
}

namespace
{
    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng){
            int r = 0;

            asDWORD oldMask = eng->SetDefaultAccessMask(0xBEEF);

            r = eng->RegisterGlobalFunction("void ObjectType(string&in)", asFUNCTION(objectType), asCALL_CDECL); assert(r >= 0);

            eng->SetDefaultAccessMask(oldMask);
        });

        return true;
    }

}

bool test = Reg();
