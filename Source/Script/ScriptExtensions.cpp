#include "ScriptExtensions.hpp"
#include "SFML/Extensions.hpp"

#include <angelscript.h>

#include <scriptarray/scriptarray.h>
#include <scriptdictionary/scriptdictionary.h>
#include <scriptfile/scriptfile.h>
#include <scriptgrid/scriptgrid.h>
#include <scriptmath/scriptmath.h>
#include <scriptmath/scriptmathcomplex.h>
#include <scriptstdstring/scriptstdstring.h>

#include <iostream>
#include <sstream>

using namespace Script;

std::list<std::pair<int, ScriptExtensions::ExtensionRegisterCallback>>* ScriptExtensions::Extensions = nullptr;

void ScriptExtensions::AddExtension(const ExtensionRegisterCallback& callback, int order)
{
    if (!Extensions)
        Extensions = new std::list<std::pair<int, ScriptExtensions::ExtensionRegisterCallback>>();

    if (Extensions->empty())
    {
        Extensions->push_back(std::make_pair(order, callback));
        return;
    }

    auto it = Extensions->begin();
    for (; it != Extensions->end(); ++it)
    {
        if (it->first > order)
            break;
    }

    Extensions->insert(it, std::make_pair(order, callback));
}

void ScriptExtensions::RegisterAll(asIScriptEngine* eng)
{
    for (auto& it : *Extensions)
        it.second(eng);
}

bool ScriptExtensions::RegisteredAllExtensions()
{
    return CommonExtensions && CommonMath && Rect && Spinor && Vector2 && InputExtensions && ScriptManagerExtensions && SFML::Extensions::RegisteredExtensions();
}

namespace
{
    template<typename T>
    void print(T in)
    {
        std::cout << in;
    }

    void printany(const void* ref, int type)
    {
        asIScriptContext* ctx = asGetActiveContext();
        asIObjectType* objType = ctx->GetEngine()->GetObjectTypeById(type);
        asIScriptFunction* func = objType->GetMethodByDecl("string ToString()");
        std::string output;

        if (func)
        {
            asIScriptContext* callCtx = ctx->GetEngine()->RequestContext();
            callCtx->Prepare(func);
            callCtx->SetObject(const_cast<void*>(ref));
            callCtx->Execute();

            output = *reinterpret_cast<std::string*>(callCtx->GetReturnAddress());

            ctx->GetEngine()->ReturnContext(callCtx);
        }
        else
        {
            std::ostringstream oss;

            if (type & asTYPEID_OBJHANDLE)
                oss << "@";

            oss << objType->GetName();

            if (type & asTYPEID_SCRIPTOBJECT)
                oss << "[SO]";
            
            oss << "{" << ref << "}";

            output = oss.str();
        }

        std::cout << output;
    }

    template<typename T>
    void println(T in)
    {
        std::cout << in << std::endl;;
    }

    void printlnany(void* ref, int type)
    {
        printany(ref, type);
        std::cout << std::endl;
    }

    void println()
    {
        std::cout << std::endl;
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
        }, -1000);

        ScriptExtensions::AddExtension([](asIScriptEngine* eng){
            int r = 0;
            
            r = eng->RegisterGlobalFunction("void print(int)", asFUNCTION(print<int>), asCALL_CDECL);
            r = eng->RegisterGlobalFunction("void print(float)", asFUNCTION(print<float>), asCALL_CDECL);
            r = eng->RegisterGlobalFunction("void print(string&in)", asFUNCTION(print<const std::string&>), asCALL_CDECL);
            r = eng->RegisterGlobalFunction("void print(const ?&in)", asFUNCTION(printany), asCALL_CDECL);
            r = eng->RegisterGlobalFunction("void println()", asFUNCTIONPR(println, (), void), asCALL_CDECL);
            r = eng->RegisterGlobalFunction("void println(int)", asFUNCTION(println<int>), asCALL_CDECL);
            r = eng->RegisterGlobalFunction("void println(float)", asFUNCTION(println<float>), asCALL_CDECL);
            r = eng->RegisterGlobalFunction("void println(string&in)", asFUNCTION(println<const std::string&>), asCALL_CDECL);
            r = eng->RegisterGlobalFunction("void println(const ?&in)", asFUNCTION(printlnany), asCALL_CDECL);
        });

        return true;
    }
}

bool ScriptExtensions::CommonExtensions = Reg();
