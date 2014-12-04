#include "ScriptHooks.hpp"
#include "ScriptExtensions.hpp"

#include <algorithm>

#include <cassert>

using Script::ScriptHooks;

std::unordered_map<std::string, std::string> ScriptHooks::mAvailableHooks;
std::unordered_map<std::string, std::set<ScriptHooks::ObjectHook> > ScriptHooks::mBoundHooks;
std::unordered_map<asIObjectType*, std::list<ScriptHooks::PossibleHook> > ScriptHooks::mPossibleBinds;

void ScriptHooks::addHook(const std::string& name, const std::string& decl)
{
    mAvailableHooks[name] = decl;
}

void ScriptHooks::storePossible(const std::string& name, asIObjectType* type, asIScriptFunction* func, float priority)
{
    if (mAvailableHooks.count(name) == 0)
        return;

    std::string funcDecl = func->GetDeclaration(false);
    size_t pos = funcDecl.find(func->GetName());
    assert(pos != std::string::npos);

    funcDecl.replace(pos, strlen(func->GetName()), "f");

    if (funcDecl != mAvailableHooks[name])
        return;

    mPossibleBinds[type].push_back(PossibleHook{name, priority, func});
}

void ScriptHooks::bindHook(const std::string& name, asIScriptObject* obj, asIScriptFunction* func, float priority)
{
    auto& hooks = mBoundHooks[name];

    hooks.insert(ObjectHook{ priority, obj, func });
}

void ScriptHooks::unbindHook(const std::string& name, asIScriptObject* obj, asIScriptFunction* func)
{
    auto& hooks = mBoundHooks[name];

    auto it = std::find_if(hooks.begin(), hooks.end(), [&](const ObjectHook& hook) { return hook.Object == obj && (func ? hook.Function == func : true); });
    if (it != hooks.end())
        hooks.erase(it);
}
void ScriptHooks::bindObject(asIScriptObject* obj)
{
    if (mPossibleBinds.count(obj->GetObjectType()) == 0)
        return;

    for (auto& bind : mPossibleBinds[obj->GetObjectType()])
    {
        bindHook(bind.HookName, obj, bind.Function, bind.Priority);
    }
}
void ScriptHooks::unbindObject(asIScriptObject* obj)
{
    for (auto& hooks : mBoundHooks)
    {
        for (auto it = hooks.second.begin(); it != hooks.second.end();)
        {
            if (it->Object == obj)
                it = hooks.second.erase(it);
            else
                ++it;
        }
    }
}

float ScriptHooks::getHookPriority(const std::string& name, asIScriptObject* obj, asIScriptFunction* func)
{
    auto& hooks = mBoundHooks[name];

    auto it = std::find_if(hooks.begin(), hooks.end(), [&](const ObjectHook& hook) { return hook.Object == obj && (func ? hook.Function == func : true); });
    if (it != hooks.end())
        return it->Priority;

    return 0;
}
void ScriptHooks::setHookPriority(const std::string& name, asIScriptObject* obj, asIScriptFunction* func, float priority)
{
    unbindHook(name, obj, func);
    bindHook(name, obj, func, priority);
}

bool ScriptHooks::ObjectHook::operator==(const ObjectHook& rhs) const
{
    return Function == Function && Object == Object && Priority == rhs.Priority;
}
bool ScriptHooks::ObjectHook::operator<(const ObjectHook& rhs) const
{
    return Priority < rhs.Priority;
}

namespace
{
    void bindHook(const std::string& name, float priority)
    {
        auto* mod = asGetActiveContext()->GetEngine()->GetModule("MetaMod");

        asIScriptFunction* func = reinterpret_cast<asIScriptFunction*>(mod->GetUserData());

        ScriptHooks::storePossible(name, func->GetObjectType(), func, priority);
    }

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng){
            int r = 0;

            r = eng->SetDefaultNamespace("Hook"); assert(r >= 0);

            asDWORD oldMask = eng->SetDefaultAccessMask(0xBEEF);

            r = eng->RegisterGlobalFunction("void BindHook(string&in,float=0)", asFUNCTION(bindHook), asCALL_CDECL); assert(r >= 0);

            eng->SetDefaultAccessMask(oldMask);

            r = eng->SetDefaultNamespace(""); assert(r >= 0);
        });

        return true;
    }
}

bool Script::ScriptExtensions::ScriptHookExtensions = Reg();
