#pragma once

#include <cassert>

using namespace Util;

namespace
{
    template <typename T>
    class has_openFromFile
    {
    private:
        template <typename U, U>
        class check
        { };

        template <typename C>
        static char f(check<bool (C::*)(const std::string&), &C::openFromFile>*);

        template <typename C>
        static long f(...);

    public:
        static const bool value = (sizeof(f<T>(0)) == sizeof(char));
    };

    template<typename T>
    struct FileLoader
    {
        static bool LoadObj(T* obj, const std::string& file) { return obj->loadFromFile(file); }
    };

    template<typename T>
    struct FileOpener
    {
        static bool LoadObj(T* obj, const std::string& file) { return obj->openFromFile(file); }
    };


    template<typename T>
    struct FileLoadingStrategy : public LoadingStrategy<T>, public std::conditional<has_openFromFile<T>::value, FileOpener<T>, FileLoader<T>>::type
    {
        FileLoadingStrategy(const std::string& file) : mFile(file) { }
        bool Load(T* obj) { return LoadObj(obj, mFile); }

        std::string mFile;
    };
}

template<typename Stored, typename Identifier>
Resource<Stored, typename Identifier>::Resource(const Identifier& id, ResourceManager<Stored, Identifier>& man) :
    mStored(nullptr), mId(id), mManager(man)
{

}

template<typename Stored, typename Identifier>
Resource<Stored, typename Identifier>::~Resource()
{
    if (mStored)
        delete mStored;
    if (mStrategy)
        delete mStrategy;
}
template<typename Stored, typename Identifier>
Stored& Resource<Stored, typename Identifier>::operator*() const
{
    return *mStored;
}

template<typename Stored, typename Identifier>
Resource<Stored, typename Identifier>::operator Stored*() const
{
    return mStored;
}

template<typename Stored, typename Identifier>
void Resource<Stored, typename Identifier>::GCRelease()
{
    if (--mRefs == 0)
    {   
        delete mStored;
        mStored = nullptr;
    }
}

template<typename Stored, typename Identifier>
bool Resource<Stored, typename Identifier>::loaded() const
{
    return mStored != nullptr;
}
template<typename Stored, typename Identifier>
bool Resource<Stored, typename Identifier>::load()
{
    mStored = new Stored();
    if (mStrategy->Load(mStored))
    {
        return true;
    }

    delete mStored;
    mStored = nullptr;
    return false;
}

template<typename ResourceType, typename Identifier>
ResourceManager<typename ResourceType, typename Identifier>::ResourceManager()
{

}
template<typename ResourceType, typename Identifier>
ResourceManager<typename ResourceType, typename Identifier>::~ResourceManager()
{

}

template<typename ResourceType, typename Identifier>
void ResourceManager<typename ResourceType, typename Identifier>::add(const Identifier& id, const std::string& file)
{
    Resource<ResourceType, Identifier>* res = new Resource<ResourceType, Identifier>(id, *this);
    res->mStrategy = new FileLoadingStrategy<ResourceType>(file);

    mResources[id] = res;
}

template<typename ResourceType, typename Identifier>
template<typename... Args>
void ResourceManager<typename ResourceType, typename Identifier>::add(const Identifier& id, const std::string& file, Args... args)
{

}
template<typename ResourceType, typename Identifier>
void ResourceManager<typename ResourceType, typename Identifier>::remove(const Identifier& id)
{
    if (mResources.count(id) == 0)
        return;

    auto obj = mResources[id];
    delete obj;

    mResources.erase(id);
}
template<typename ResourceType, typename Identifier>
bool ResourceManager<typename ResourceType, typename Identifier>::has(const Identifier& id) const
{
    return mResources.count(id) > 0;
}

template<typename ResourceType, typename Identifier>
Resource<ResourceType, Identifier>* ResourceManager<typename ResourceType, typename Identifier>::get(const Identifier& id)
{
    Resource<ResourceType, Identifier>* res = mResources.at(id);

    if (res->loaded())
    {
        res->GCAddRef();
        return res;
    }

    res->load();
    assert(res->loaded());

    return res;
}