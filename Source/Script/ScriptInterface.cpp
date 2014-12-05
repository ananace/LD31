#include "ScriptInterface.hpp"

#include <Util/FileSystem.hpp>

#include <nowide/fstream.hpp>
#include <angelscript.h>

#include <map>

void Script::DumpScriptInterface(asIScriptEngine* eng)
{
    nowide::ofstream ofs;

    ofs.open("ScriptInterface.md", std::ios::trunc);
    if (!ofs || !ofs.is_open())
        return;

    ofs << "LD31 Script interface documentation" << std::endl
        << "===================================" << std::endl << std::endl;

    ofs << "Funcdefs" << std::endl
        << "--------" << std::endl;

    for (uint32_t i = 0; i < eng->GetFuncdefCount(); ++i)
    {
        auto* func = eng->GetFuncdefByIndex(i);

        ofs << "    " << std::string(func->GetDeclaration(true, true, true)) << std::endl;
    }

    ofs << std::endl
        << "Global functions" << std::endl
        << "----------------" << std::endl;

    for (uint32_t i = 0; i < eng->GetGlobalFunctionCount(); ++i)
    {
        auto* func = eng->GetGlobalFunctionByIndex(i);

        ofs << "    " << std::string(func->GetDeclaration(true, true, true)) << std::endl;
    }

    ofs << std::endl
        << "Script Objects" << std::endl
        << "--------------" << std::endl;

    std::map<std::string, asIObjectType*> objs;
    for (uint32_t i = 0; i < eng->GetObjectTypeCount(); ++i)
    {
        auto* objType = eng->GetObjectTypeByIndex(i);
        if (!objType)
            continue;

        objs[objType->GetName()] = objType;
    }

    for (auto& name : objs)
    {
        auto* objType = name.second;

        ofs << "### ";
        std::string ns = objType->GetNamespace();
        if (!ns.empty())
            ofs << ns << "::";
        ofs << std::string(objType->GetName()) << " ###" << std::endl;

        if (objType->GetPropertyCount() > 0)
        {
            ofs << std::endl << "Properties:" << std::endl << std::endl;
            
            for (uint32_t j = 0; j < objType->GetPropertyCount(); ++j)
            {
                ofs << "    " << std::string(objType->GetPropertyDeclaration(j)) << std::endl;
            }
        }

        ofs << std::endl;
        ofs << "Functions:" << std::endl << std::endl;

        for (uint32_t j = 0; j < objType->GetMethodCount(); ++j)
        {
            auto* func = objType->GetMethodByIndex(j);
            ofs << "    " << std::string(func->GetDeclaration(false, false, true)) << std::endl;
        }

        ofs << std::endl << "--------" << std::endl << std::endl;
    }
}
