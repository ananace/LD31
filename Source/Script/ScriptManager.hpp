#pragma once

#include <Util/Time.hpp>

#include <scriptbuilder/scriptbuilder.h>

#include <functional>
#include <string>
#include <tuple>
#include <vector>

class asIScriptContext;
class asIScriptEngine;
class asIScriptFunction;
class asIScriptModule;
class asIScriptObject;

struct CUserType;

namespace Script
{

static class Manager
{
public:
    typedef std::function<CUserType*()> SerializerCallback_t;

    Manager();
    ~Manager();

    void setEngine(asIScriptEngine*);

    bool loadScriptFromFile(const std::string& file);
    bool loadScriptFromMemory(const std::string& file, const char* data, size_t length);
    bool loadScriptFromMemory(const std::string& file, const std::string& data);

    ///\todo Add a timeout for the co-routine execution
    void runCoroutines();

    void defineWord(const std::string& word);

    void notifyNewObject(asIScriptObject* obj);
    void notifyObjectRemoved(asIScriptObject* obj);

    void registerType(const std::string& name, const SerializerCallback_t& type);

    asIScriptContext* addCoRoutine(asIScriptFunction* func);
    void sleep(asIScriptContext* ctx, const Util::Timespan& duration);

private:
    struct CoRoutine;

    void runMetadata(const std::string& data, asIScriptContext* ctx);

    asIScriptEngine* mEngine;
    asIScriptModule* mMetaMod;

    std::vector<std::string> mLoadedScripts;
    std::vector<asIScriptObject*> mObjects;
    std::vector<std::tuple<std::string, SerializerCallback_t>> mSerializerTypes;
    std::vector<CoRoutine*> mCoRoutines;

    CScriptBuilder mBuilder;
} ScriptManager;

}