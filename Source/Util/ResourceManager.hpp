#pragma once

#include "RefCount.hpp"

#include <string>
#include <type_traits>
#include <unordered_map>

namespace Util
{
    /// A helper class for loading resources
    template<typename T>
    struct LoadingStrategy
    {
        virtual bool Load(T* obj) { return false; }
    };

template<typename ResourceType, typename Identifier>
class ResourceManager;

/// A reference counted resource type
template<typename Stored, typename Identifier>
class Resource : public RefCounted
{
public:
    ~Resource();

    /// Gets a reference to the stored resource
    Stored& operator*() const;
    /// Gets a pointer to the stored resource
    Stored* operator->() const;
    /// Casts to a pointer to the stored resource
    operator Stored*() const;

    /// Gets the identifier of the stored resource
    Identifier getId() const;

    /// Releases a reference, and frees the stored resource if it runs out
    void GCRelease();

private:
    /// Construct the resource from an ID and a manager reference
    Resource(const Identifier& id, ResourceManager<Stored, Identifier>& man);
    
    /// The strategy for loading the stored resource
    LoadingStrategy<Stored>* mStrategy;

    /// Is the resource loaded?
    bool loaded() const;
    /// Loads the stored resource
    bool load();

    /// A pointer to the stored resource, or null if not loaded
    Stored* mStored;
    /// The identifier of the stored resource
    Identifier mId;
    /// A reference to the owning manager
    ResourceManager<Stored, Identifier>& mManager;

    friend class ResourceManager<Stored, Identifier>;
};

/** \brief A lazy-loading resource manager
 */
template<typename ResourceType, typename Identifier = std::string>
class ResourceManager
{
public:
    /// Default constructor
    ResourceManager() = default;
    /// No copying
    ResourceManager(const ResourceManager&) = delete;
    /// Moving constructor
    ResourceManager(ResourceManager&&);
    /// Destructor
    ~ResourceManager();

    /// No assignments
    ResourceManager& operator=(const ResourceManager&) = delete;

    /// Adds a resource with the given identifier and source file
    void add(const Identifier& id, const std::string& file);
    /// Adds a resource with the given identifier, loading arguments, and source file
    template<typename... Args>
    void add(const Identifier& id, const std::string& file, Args... args);
    /// Removes a resource from the manager, any references to it will stay alive
    void remove(const Identifier& id);
    /// Does the resource manager have a resource with the given identifier
    bool has(const Identifier& id) const;
    
    /// Gets a pointer to the given resource, loading it if it's not already loaded
    Resource<ResourceType, Identifier>* get(const Identifier& id);

private:
    /// The list of current resources
    std::unordered_map<std::string, Resource<ResourceType, Identifier>*> mResources;
};

}

#include "ResourceManager.inl"
