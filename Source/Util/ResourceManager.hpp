#pragma once

#include "RefCount.hpp"

#include <string>
#include <type_traits>
#include <unordered_map>

namespace Util
{
    template<typename T>
    struct LoadingStrategy
    {
        virtual bool Load(T* obj) { return false; }
    };

template<typename ResourceType, typename Identifier>
class ResourceManager;

template<typename Stored, typename Identifier>
class Resource : public RefCounted
{
public:
    ~Resource();
    Stored& operator*() const;
    operator Stored*() const;

    void GCRelease();

private:
    Resource(const Identifier& id, ResourceManager<Stored, Identifier>& man);
    
    LoadingStrategy<Stored>* mStrategy;

    bool loaded() const;
    bool load();

    Stored* mStored;
    Identifier mId;
    ResourceManager<Stored, Identifier>& mManager;

    friend class ResourceManager<Stored, Identifier>;
};

template<typename ResourceType, typename Identifier = std::string>
class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    void add(const Identifier& id, const std::string& file);
    template<typename... Args>
    void add(const Identifier& id, const std::string& file, Args... args);
    void remove(const Identifier& id);
    bool has(const Identifier& id) const;

    Resource<ResourceType, Identifier>* get(const Identifier& id);

private:
    std::unordered_map<std::string, Resource<ResourceType, Identifier>*> mResources;
};

}

#include "ResourceManager.inl"
