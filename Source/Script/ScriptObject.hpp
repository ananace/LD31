#pragma once

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

private:
    asIScriptModule* mModule;
    asIScriptObject* mObject;
};

}
