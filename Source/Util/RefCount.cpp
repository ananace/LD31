#include "RefCount.hpp"

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
