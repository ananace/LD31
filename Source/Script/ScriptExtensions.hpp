#pragma once

#include <functional>
#include <list>

class asIScriptEngine;

namespace Script
{

struct ScriptExtensions
{
    typedef std::function<void(asIScriptEngine*)> ExtensionRegisterCallback;

    static void AddExtension(const ExtensionRegisterCallback& callback, int priority = 0);
    static void RegisterAll(asIScriptEngine* eng);

    static bool CommonExtensions;

private:
    static std::list<std::pair<int, ExtensionRegisterCallback>> Extensions;
};

}
