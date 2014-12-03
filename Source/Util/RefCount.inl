#include <angelscript.h>
#include <cassert>

using namespace Util;

template<typename T>
void RefCounted::RegisterBehaviours(asIScriptEngine* eng, const std::string& obj)
{
    static_assert(std::is_base_of<RefCounted, T>::value, "Can only register reference counting behaviours on a reference counted class");

    const char* name = obj.c_str();
    int r = 0;

    r = eng->RegisterObjectBehaviour(name, asBEHAVE_ADDREF, "void f()", asMETHOD(T, GCAddRef), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectBehaviour(name, asBEHAVE_RELEASE, "void f()", asMETHOD(T, GCRelease), asCALL_THISCALL); assert(r >= 0);
}

template<typename T>
void GarbageCollected::RegisterBehaviours(asIScriptEngine* eng, const std::string& obj)
{
    static_assert(std::is_base_of<RefCounted, T>::value, "Can only register garbage collection behaviours on a garbage collected class");

    const char* name = obj.c_str();
    int r = 0;

    r = eng->RegisterObjectBehaviour(name, asBEHAVE_ADDREF, "void f()", asMETHOD(T, GCAddRef), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectBehaviour(name, asBEHAVE_RELEASE, "void f()", asMETHOD(T, GCRelease), asCALL_THISCALL); assert(r >= 0);

    r = eng->RegisterObjectBehaviour(name, asBEHAVE_SETGCFLAG, "void f()", asMETHOD(T, GCSetFlag), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectBehaviour(name, asBEHAVE_GETGCFLAG, "bool f()", asMETHOD(T, GCGetFlag), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectBehaviour(name, asBEHAVE_GETREFCOUNT, "int f()", asMETHOD(T, GCGetRefCount), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectBehaviour(name, asBEHAVE_ENUMREFS, "void f(int&in)", asMETHOD(T, GCEnumRefs), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectBehaviour(name, asBEHAVE_RELEASEREFS, "void f(int&in)", asMETHOD(T, GCReleaseRefs), asCALL_THISCALL); assert(r >= 0);
}
