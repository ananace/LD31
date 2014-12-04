#pragma once

#include <Util/Time.hpp>

#include <scriptbuilder/scriptbuilder.h>

#include <functional>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

class asIScriptContext;
class asIScriptEngine;
class asIScriptFunction;
class asIScriptModule;
class asIScriptObject;

struct CUserType;

namespace Script
{

extern class Manager
{
public:
    typedef std::function<CUserType*()> SerializerCallback_t;

    Manager();
    ~Manager();

    void setEngine(asIScriptEngine*);

    bool loadScriptFromFile(const std::string& file);
    bool loadScriptFromMemory(const std::string& file, const char* data, size_t length);
    bool loadScriptFromMemory(const std::string& file, const std::string& data);

    void checkForModification();
    void runCoroutines(const Util::Timespan& duration = std::chrono::milliseconds(5));

    void defineWord(const std::string& word);

    void notifyIncluded(const std::string& script, const std::string& from);
    void notifyNewObject(asIScriptObject* obj);
    void notifyObjectRemoved(asIScriptObject* obj);

    void registerType(const std::string& name, const SerializerCallback_t& type);

    asIScriptContext* addCoRoutine(asIScriptFunction* func);
    void sleep(asIScriptContext* ctx, const Util::Timespan& duration);

private:
    struct CoRoutine;
    struct LoadedScriptData
    {
        LoadedScriptData();

        std::set<std::string> IncludedFrom;
        Util::Timestamp LastModified;
        bool Loaded;
    };

    void runMetadata(const std::string& data, asIScriptContext* ctx);

    asIScriptEngine* mEngine;
    asIScriptModule* mMetaMod;

    Util::Timestamp mLastModificationCheck;
    std::unordered_map<std::string, LoadedScriptData> mLoadedScripts;

    std::vector<asIScriptObject*> mObjects;
    std::vector<std::tuple<std::string, SerializerCallback_t>> mSerializerTypes;
    std::vector<CoRoutine*> mCoRoutines;

    CScriptBuilder mBuilder;
} ScriptManager;

}