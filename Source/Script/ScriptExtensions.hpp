#pragma once

#include <functional>
#include <list>

class asIScriptEngine;

namespace Script
{

static struct ScriptExtensionStore
{
    typedef std::function<void(asIScriptEngine*)> ExtensionRegisterCallback;

    void AddExtension(const ExtensionRegisterCallback& callback, bool preregister = false);

    void RegisterAll(asIScriptEngine* eng);

private:
    std::list<ExtensionRegisterCallback> Extensions;
} ScriptExtensions;

}
