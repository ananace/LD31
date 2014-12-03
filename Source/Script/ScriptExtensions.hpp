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

    static bool RegisteredAllExtensions();

private:
    static std::list<std::pair<int, ExtensionRegisterCallback>>* Extensions;

    static bool CommonExtensions;
    static bool CommonMath;
    static bool Rect;
    static bool Spinor;
    static bool Vector2;

    static bool InputExtensions;
    static bool ScriptManagerExtensions;
};

}
