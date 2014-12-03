#pragma once

#include <atomic>
#include <string>

class asIScriptEngine;

namespace Util
{

/// Inherit this to handle memory by ref counting
class RefCounted
{
public:
    /// Construct a ref counted value, sets the number of refs to 1
    RefCounted();
    /// Destroys a ref counted value
    virtual ~RefCounted();

    /// Adds a ref to the counter
    virtual void GCAddRef();
    /// Removes a ref from the counter, and destroys the object if the refs run out
    virtual void GCRelease();

    /// Register script behaviours for the given object type
    template<typename T>
    static void RegisterBehaviours(asIScriptEngine* eng, const std::string& name);

protected:
    std::atomic_int mRefs; ///< The number of references
};

/// Inherit this to handle memory by ref counting and garbage collection
class GarbageCollected : public RefCounted
{
public:
    /// Default constructor
    GarbageCollected();
    /// Default destructor
    virtual ~GarbageCollected();

    /// Adds a ref
    virtual void GCAddRef();
    /// Removes a ref
    virtual void GCRelease();

    /// Sets the GC flag
    virtual void GCSetFlag();
    /// Gets the GC flag
    virtual bool GCGetFlag() const;
    /// Gets the number of references
    virtual int GCGetRefCount() const;

    /// Enumerates all references and calls eng->GCEnumCallback for each reference
    virtual void GCEnumRefs(asIScriptEngine* eng);
    /// Releases the memory for all references
    virtual void GCReleaseRefs(asIScriptEngine* eng);

    /// Register GC and ref counting behaviours for the given object type
    template<typename T>
    static void RegisterBehaviours(asIScriptEngine* eng, const std::string& name);

private:
    bool mGCFlag; ///< THe GC flag
};

}

#include "RefCount.inl"
