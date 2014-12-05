#include "ScriptInterface.hpp"

#include <Util/FileSystem.hpp>

#include <nowide/fstream.hpp>
#include <angelscript.h>

#include <map>

namespace
{
    struct ci_less : public std::binary_function < std::string, std::string, bool >
    {
        struct nocase_compare : public std::binary_function < char, char, bool >
        {
            bool operator()(const char& a, const char& b)
            {
                return tolower(a) < tolower(b);
            }
        };

        bool operator()(const std::string& str1, const std::string& str2)
        {
            return std::lexicographical_compare(str1.begin(), str1.end(), str2.begin(), str2.end(), nocase_compare());
        }
    };
}

void dumpType(std::ostream& ofs, asIObjectType* objType, uint8_t level = 0)
{
    auto* eng = objType->GetEngine();
    
    for (uint8_t i = 0; i < level; ++i)
        ofs << "#";
    ofs << "#### ";
    std::string ns = objType->GetNamespace();
    if (!ns.empty())
        ofs << ns << "::";
    ofs << std::string(objType->GetName());
    
    if (objType->GetSubTypeCount() > 0)
    {
        ofs << "\\<";

        for (uint32_t j = 0; j < objType->GetSubTypeCount(); ++j)
        {
            auto* type = objType->GetSubType(j);

            if (j > 0)
                ofs << ", ";

            ofs << std::string(type->GetName());
        }

        ofs << "\\>";
    }

    ofs << " ####";
    for (uint8_t i = 0; i < level; ++i)
        ofs << "#";
    ofs << std::endl << std::endl;

    std::vector<std::string> factories;
    std::map<std::string, std::string, ci_less> methods;
    std::vector<std::string> operators;
    std::map<std::string, std::string, ci_less> properties;

    for (uint32_t j = 0; j < objType->GetFactoryCount(); ++j)
    {
        auto* func = objType->GetFactoryByIndex(j);

        factories.push_back(func->GetDeclaration(false, false, true));
    }

    for (uint32_t j = 0; j < objType->GetBehaviourCount(); ++j)
    {
        asEBehaviours beh;
        auto* func = objType->GetBehaviourByIndex(j, &beh);

        if (beh == asBEHAVE_CONSTRUCT)
            factories.push_back(func->GetDeclaration(false, false, true));
    }

    for (uint32_t j = 0; j < objType->GetPropertyCount(); ++j)
    {
        const char* name;
        bool isPrivate;

        objType->GetProperty(j, &name, nullptr, &isPrivate);
        if (!isPrivate)
            properties[name] = objType->GetPropertyDeclaration(j);
    }

    for (uint32_t j = 0; j < objType->GetMethodCount(); ++j)
    {
        auto* func = objType->GetMethodByIndex(j);

        std::string name = func->GetName();
        if (name.substr(0, 2) == "op")
            operators.push_back(func->GetDeclaration(false, false, true));
        else if (name.substr(0, 4) == "get_")
        {
            std::string setname = name;
            setname.replace(0, 4, "set_");
            name.erase(0, 4);
            int typeId = func->GetReturnTypeId();
            bool readOnly = objType->GetMethodByName(setname.c_str()) == nullptr;

            auto* retType = eng->GetObjectTypeById(typeId);
            const char* type = eng->GetTypeDeclaration(typeId, (retType && retType->GetNamespace() != objType->GetNamespace()));

            properties[name] = std::string(type) + " " + name + (readOnly ? " const" : "");
        }
        else if (name.substr(0, 4) == "set_")
            ;
        else
            methods[name] = func->GetDeclaration(false, false, true);
    }

    if (!factories.empty())
    {
        ofs << "Factories:" << std::endl << std::endl;

        for (auto& fac : factories)
            ofs << "    " << fac << std::endl;

        ofs << std::endl;
    }

    if (!properties.empty())
    {
        ofs << "Properties:" << std::endl << std::endl;

        for (auto& prop : properties)
        {
            ofs << "    " << prop.second << std::endl;
        }

        ofs << std::endl;
    }

    if (!operators.empty())
    {
        ofs << "Operators: " << std::endl << std::endl;

        for (auto& op : operators)
        {
            ofs << "    " << op << std::endl;
        }

        ofs << std::endl;
    }

    if (!methods.empty())
    {
        ofs << "Methods:" << std::endl << std::endl;

        for (auto& meth : methods)
        {
            ofs << "    " << meth.second << std::endl;
        }

        ofs << std::endl;
    }

    if (factories.empty() && properties.empty() && operators.empty() && methods.empty())
        ofs << "##### Empty class #####" << std::endl;

    ofs << std::endl;
    if (level == 0)
        ofs << "--------" << std::endl << std::endl;
}

void Script::DumpScriptInterface(asIScriptEngine* eng)
{
    nowide::ofstream ofs;

    ofs.open("ScriptInterface.md", std::ios::trunc);
    if (!ofs || !ofs.is_open())
        return;

    ofs << "LD31 Script interface documentation" << std::endl
        << "===================================" << std::endl << std::endl;

    ofs << "Global properties" << std::endl
        << "-----------------" << std::endl;

    std::map<std::string, std::vector<std::string> > functions;
    std::map<std::string, std::vector<std::string> > properties;

    for (uint32_t i = 0; i < eng->GetGlobalFunctionCount(); ++i)
    {
        auto* func = eng->GetGlobalFunctionByIndex(i);
        std::string name = func->GetName();

        if (name.substr(0, 4) == "get_")
        {
            std::string setname = name;
            setname.replace(0, 4, "set_");
            name.erase(0, 4);
            
            int typeId = func->GetReturnTypeId();
            auto* retType = eng->GetObjectTypeById(typeId);
            const char* retName = eng->GetTypeDeclaration(typeId);

            bool readOnly = eng->GetGlobalFunctionByDecl(("void " + setname + "(" + retName + ")").c_str()) == nullptr;

            properties[func->GetNamespace()].push_back(std::string(retName) + " " + (func->GetNamespace() ? std::string(func->GetNamespace()) + "::" : "") + name + (readOnly ? " const" : ""));
        }
        else if (name.substr(0, 4) == "set_")
            ;
        else
            functions[func->GetNamespace()].push_back(std::string(func->GetDeclaration(true, true, true)));
    }

    for (uint32_t i = 0; i < eng->GetGlobalPropertyCount(); ++i)
    {
        const char* name,
                  * ns;
        int typeId;
        bool isConst;
        eng->GetGlobalPropertyByIndex(i, &name, &ns, &typeId, &isConst);

        auto* retType = eng->GetTypeDeclaration(typeId);

        properties[ns].push_back(std::string(retType) + " " + (ns ? std::string(ns) + "::" : "") + name + (isConst ? " const" : ""));
    }

    for (auto& func : properties)
    {
        if (!func.first.empty())
            ofs << std::endl << "#### " << func.first << ":" << " ####" << std::endl << std::endl;

        for (auto& f : func.second)
            ofs << "    " << f << std::endl;
    }

    ofs << std::endl
        << "Funcdefs" << std::endl
        << "--------" << std::endl;

    for (uint32_t i = 0; i < eng->GetFuncdefCount(); ++i)
    {
        auto* func = eng->GetFuncdefByIndex(i);

        ofs << "    " << std::string(func->GetDeclaration(true, true, true)) << std::endl;
    }

    ofs << std::endl
        << "Global functions" << std::endl
        << "----------------" << std::endl;

    for (auto& func : functions)
    {
        if (!func.first.empty())
            ofs << std::endl << "#### " << func.first << ":" << " ####" << std::endl << std::endl;

        for (auto& f : func.second)
            ofs << "    " << f << std::endl;
    }

    ofs << std::endl
        << "Script Objects" << std::endl
        << "--------------" << std::endl;

    std::map<std::string, std::map<std::string, asIObjectType*, ci_less> > objs;
    for (uint32_t i = 0; i < eng->GetObjectTypeCount(); ++i)
    {
        auto* objType = eng->GetObjectTypeByIndex(i);
        if (!objType)
            continue;

        objs[objType->GetNamespace()][objType->GetName()] = objType;
    }

    for (auto& ns : objs)
    {
        if (!ns.first.empty())
            ofs << std::endl << "### " << ns.first << ":" << " ###" << std::endl << std::endl;

        for (auto& name : ns.second)
        {
            auto* objType = name.second;

            dumpType(ofs, objType);
        }
    }
}
