#include "ScriptObject.hpp"
#include "ScriptManager.hpp"

#include <angelscript.h>

#include <scriptdictionary/scriptdictionary.h>

using namespace Script;

ScriptObject::ScriptObject(asIObjectType* type)
{
    auto* eng = type->GetEngine();

    mObject = reinterpret_cast<asIScriptObject*>(eng->CreateScriptObject(type));
    type->SetUserData(this, (uintptr_t)mObject);

    mModule = mObject->GetObjectType()->GetModule();
    ScriptManager* scriptMan = reinterpret_cast<ScriptManager*>(mModule->GetUserData());
    scriptMan->notifyNewObject(mObject);
}

ScriptObject::~ScriptObject()
{
    ScriptManager* scriptMan = reinterpret_cast<ScriptManager*>(mModule->GetUserData());
    scriptMan->notifyObjectRemoved(mObject);
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

    mObject->GetObjectType()->SetUserData(nullptr, (uintptr_t)mObject);

    mObject = newObject;
    
    mObject->GetObjectType()->SetUserData(this, (uintptr_t)mObject);

    oldObj->Release();
    oldType->Release();

    func = mObject->GetObjectType()->GetMethodByDecl("void PostReload(dictionary@)");
    if (func)
    {
        ctx->Prepare(func);
        ctx->SetObject(mObject);
        ctx->SetArgObject(0, dict);

        int r = ctx->Execute();
    }

    mObject->GetEngine()->ReturnContext(ctx);
    dict->Release();
}