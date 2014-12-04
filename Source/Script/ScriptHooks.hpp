#pragma once

#include <set>
#include <string>
#include <unordered_map>

class asIObjectType;
class asIScriptEngine;
class asIScriptFunction;
class asIScriptObject;

namespace Script
{

class ScriptHooks
{
public:
    ///\brief Add a hook with the specified name and declaration.
    ///
    /// The declaration is used for type checking the function arguments and return value.
    static void addHook(const std::string& name, const std::string& decl);

    static void storePossible(const std::string& name, asIObjectType* obj, asIScriptFunction* func, float priority = 0);
    /// Binds the named hook to the specified object function, with the given priority
    static void bindHook(const std::string& name, asIScriptObject* obj, asIScriptFunction* func, float priority = 0);
    /// Unbinds the named hook from the given object, (And all functions unless a specific one is given)
    static void unbindHook(const std::string& name, asIScriptObject* obj, asIScriptFunction* func = nullptr);

    /// Bind all hooks to the given object
    static void bindObject(asIScriptObject* obj);
    /// Removes all hooks bound to the given object
    static void unbindObject(asIScriptObject* obj);
    
    /// Gets the hook priority for the given name, object, and function/functions
    static float getHookPriority(const std::string& name, asIScriptObject* obj, asIScriptFunction* func = nullptr);
    /// Set the hook priority for the given name, object, and function/functions
    static void setHookPriority(const std::string& name, asIScriptObject* obj, asIScriptFunction* func, float priority);

    /// Call the named hook, with the given arguments
    template<typename... Args>
    static void execute(const std::string& name, asIScriptEngine* eng, Args... args);

private:
    /** \brief Storage type for one bound hook
     *
     * 
     */
    struct ObjectHook
    {
        /// Priority of the hook, execution runs from lowest and upwards
        float Priority;
        asIScriptObject* Object;
        asIScriptFunction* Function;

        bool operator==(const ObjectHook&) const;
        bool operator<(const ObjectHook&) const;
    };

    struct PossibleHook
    {
        std::string HookName;
        float Priority;
        asIScriptFunction* Function;
    };

    static std::unordered_map<std::string, std::string> mAvailableHooks;

    static std::unordered_map<std::string, std::set<ObjectHook> > mBoundHooks;
    static std::unordered_map<asIObjectType*, std::list<PossibleHook> > mPossibleBinds;
};

}

#include "ScriptHooks.inl"
