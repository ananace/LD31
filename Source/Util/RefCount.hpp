#pragma once

#include <atomic>
#include <string>

class asIScriptEngine;

namespace Util
{

class RefCounted
{
public:
    RefCounted();
    virtual ~RefCounted();

    virtual void GCAddRef();
    virtual void GCRelease();

    static void RegisterBehaviours(asIScriptEngine* eng, const std::string& name);

protected:
    std::atomic_int mRefs;
};

class GarbageCollected : public RefCounted
{
public:
    GarbageCollected();
    virtual ~GarbageCollected();

    virtual void GCAddRef();
    virtual void GCRelease();

    virtual void GCSetFlag();
    virtual bool GCGetFlag() const;
    virtual int GCGetRefCount() const;

    virtual void GCEnumRefs(asIScriptEngine* eng);
    virtual void GCReleaseRefs(asIScriptEngine* eng);

    static void RegisterBehaviours(asIScriptEngine* eng, const std::string& name);

private:
    bool mGCFlag;
};

}