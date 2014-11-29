#include "RefCount.hpp"

#include <angelscript.h>

#include <cassert>

using namespace Util;

RefCounted::RefCounted()
{
    mRefs.store(1);
}
RefCounted::~RefCounted()
{

}

void RefCounted::GCAddRef()
{
    ++mRefs;
}
void RefCounted::GCRelease()
{
    if (--mRefs <= 0)
        delete this;
}

void RefCounted::RegisterBehaviours(asIScriptEngine* engine, const std::string& obj)
{
    const char* name = obj.c_str();
    int r = 0;

    r = engine->RegisterObjectBehaviour(name, asBEHAVE_ADDREF, "void f()", asMETHOD(RefCounted, GCAddRef), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectBehaviour(name, asBEHAVE_RELEASE, "void f()", asMETHOD(RefCounted, GCRelease), asCALL_THISCALL); assert(r >= 0);
}

GarbageCollected::GarbageCollected() : mGCFlag(false)
{
}
GarbageCollected::~GarbageCollected()
{
}

void GarbageCollected::GCAddRef()
{
    mGCFlag = false;
    ++mRefs;
}
void GarbageCollected::GCRelease()
{
    mGCFlag = false;
    if (--mRefs <= 0)
        delete this;
}
int GarbageCollected::GCGetRefCount() const
{
    return mRefs;
}

void GarbageCollected::GCSetFlag()
{
    mGCFlag = true;
}
bool GarbageCollected::GCGetFlag() const
{
    return mGCFlag;
}

void GarbageCollected::GCEnumRefs(asIScriptEngine* eng)
{

}
void GarbageCollected::GCReleaseRefs(asIScriptEngine* eng)
{

}

void GarbageCollected::RegisterBehaviours(asIScriptEngine* engine, const std::string& obj)
{
    const char* name = obj.c_str();
    int r = 0;

    r = engine->RegisterObjectBehaviour(name, asBEHAVE_ADDREF, "void f()", asMETHOD(GarbageCollected, GCAddRef), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectBehaviour(name, asBEHAVE_RELEASE, "void f()", asMETHOD(GarbageCollected, GCRelease), asCALL_THISCALL); assert(r >= 0);

    r = engine->RegisterObjectBehaviour(name, asBEHAVE_SETGCFLAG, "void f()", asMETHOD(GarbageCollected, GCSetFlag), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectBehaviour(name, asBEHAVE_GETGCFLAG, "bool f()", asMETHOD(GarbageCollected, GCGetFlag), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectBehaviour(name, asBEHAVE_GETREFCOUNT, "int f()", asMETHOD(GarbageCollected, GCGetRefCount), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectBehaviour(name, asBEHAVE_ENUMREFS, "void f(int&in)", asMETHOD(GarbageCollected, GCEnumRefs), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectBehaviour(name, asBEHAVE_RELEASEREFS, "void f(int&in)", asMETHOD(GarbageCollected, GCReleaseRefs), asCALL_THISCALL); assert(r >= 0);
}
