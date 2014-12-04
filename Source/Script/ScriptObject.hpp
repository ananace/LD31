#pragma once

#include <string>

class asIObjectType;
class asIScriptModule;
class asIScriptObject;

namespace Script
{

class ScriptObject
{
public:
    ScriptObject(asIObjectType* type);
    ~ScriptObject();

    asIScriptObject* getObject();

    void updateObject(asIScriptObject* newObject);

    static ScriptObject* Create(const std::string& name);

private:
    asIScriptObject* mObject;
};

}
