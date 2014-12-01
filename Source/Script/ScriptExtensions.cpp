#include "ScriptExtensions.hpp"

#include <scriptarray/scriptarray.h>
#include <scriptdictionary/scriptdictionary.h>
#include <scriptfile/scriptfile.h>
#include <scriptgrid/scriptgrid.h>
#include <scriptmath/scriptmath.h>
#include <scriptmath/scriptmathcomplex.h>
#include <scriptstdstring/scriptstdstring.h>

using namespace Script;

std::list<ScriptExtensions::ExtensionRegisterCallback> ScriptExtensions::Extensions;

void ScriptExtensions::AddExtension(const ExtensionRegisterCallback& callback, bool preregister)
{
    if (preregister)
        Extensions.push_front(callback);
    else
        Extensions.push_back(callback);
}

void ScriptExtensions::RegisterAll(asIScriptEngine* eng)
{
    for (auto& it : Extensions)
        it(eng);
}

bool Reg()
{
    // Add all the default (Angelscript) extensions
    ScriptExtensions::AddExtension([](asIScriptEngine* eng){
        RegisterStdString(eng);

        RegisterScriptArray(eng, true);
        RegisterScriptDictionary(eng);
        RegisterScriptFile(eng);
        RegisterScriptGrid(eng);
        RegisterScriptMath(eng);
        RegisterScriptMathComplex(eng);
        RegisterStdStringUtils(eng);
    }, true);

    return true;
}

bool ScriptExtensions::CommonExtensions = Reg();
