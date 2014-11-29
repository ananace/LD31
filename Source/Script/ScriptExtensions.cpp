#include "ScriptExtensions.hpp"

using namespace Script;

ScriptExtensionStore ScriptExtensions;

void ScriptExtensionStore::AddExtension(const ExtensionRegisterCallback& callback, bool preregister = false)
{
    if (preregister)
        Extensions.push_front(callback);
    else
        Extensions.push_back(callback);
}

void ScriptExtensionStore::RegisterAll(asIScriptEngine* eng)
{
    for (auto& it : Extensions)
        it(eng);
}
