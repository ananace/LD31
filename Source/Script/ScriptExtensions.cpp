#include "ScriptExtensions.hpp"

#include <scriptarray/scriptarray.h>
#include <scriptdictionary/scriptdictionary.h>
#include <scriptfile/scriptfile.h>
#include <scriptgrid/scriptgrid.h>
#include <scriptmath/scriptmath.h>
#include <scriptmath/scriptmathcomplex.h>
#include <scriptstdstring/scriptstdstring.h>

using namespace Script;

ScriptExtensionStore ScriptExtensions;

void ScriptExtensionStore::AddExtension(const ExtensionRegisterCallback& callback, bool preregister)
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

ScriptExtensionStore::ScriptExtensionStore()
{
    // Add all the default (Angelscript) extensions
    AddExtension([](asIScriptEngine* eng){
        RegisterScriptArray(eng, true);
        RegisterScriptDictionary(eng);
        RegisterScriptFile(eng);
        RegisterScriptGrid(eng);
        RegisterScriptMath(eng);
        RegisterScriptMathComplex(eng);
        RegisterStdString(eng);
        RegisterStdStringUtils(eng);
    }, true);
}
