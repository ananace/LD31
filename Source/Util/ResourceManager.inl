#pragma once

#include <functional>
#include <cassert>

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

    template<typename T, typename... Args>
    struct FileLoader
    {
        static bool LoadObj(T* obj, const std::string& file, Args... args) { return obj->loadFromFile(file, args...); }
    };

    template<typename T, typename... Args>
    struct FileOpener
    {
        static bool LoadObj(T* obj, const std::string& file, Args... args) { return obj->openFromFile(file, args...); }
    };

    template<typename T, typename... Args>
    struct FileLoadingStrategy : public LoadingStrategy<T>, public std::conditional<has_openFromFile<T>::value, FileOpener<T, Args...>, FileLoader<T, Args...>>::type
    {
        FileLoadingStrategy(const std::string& file, Args... args) : mFile(file)
        {
            mLoader = [&](T* obj) {
                return LoadObj(obj, mFile, args...);
            };
        }

        bool Load(T* obj) { return mLoader(obj); }

        std::string mFile;
        std::function<bool(T*)> mLoader;
    };
}

template<typename Stored, typename Identifier>
Util::Resource<Stored, Identifier>::Resource(const Identifier& id, ResourceManager<Stored, Identifier>& man) :
    mStored(nullptr), mId(id), mManager(man)
{

}

template<typename Stored, typename Identifier>
Util::Resource<Stored, Identifier>::~Resource()
{
    if (mStored)
        delete mStored;
    if (mStrategy)
        delete mStrategy;
}
template<typename Stored, typename Identifier>
Stored& Util::Resource<Stored, Identifier>::operator*() const
{
    return *mStored;
}
template<typename Stored, typename Identifier>
Stored* Util::Resource<Stored, Identifier>::operator->() const
{
    return mStored;
}

template<typename Stored, typename Identifier>
Identifier Util::Resource<Stored, Identifier>::getId() const
{
    return mId;
}

template<typename Stored, typename Identifier>
Util::Resource<Stored, Identifier>::operator Stored*() const
{
    return mStored;
}

template<typename Stored, typename Identifier>
void Util::Resource<Stored, Identifier>::GCRelease()
{
    if (--mRefs == 0)
    {   
        delete mStored;
        mStored = nullptr;
    }
}

template<typename Stored, typename Identifier>
bool Util::Resource<Stored, Identifier>::loaded() const
{
    return mStored != nullptr;
}
template<typename Stored, typename Identifier>
bool Util::Resource<Stored, Identifier>::load()
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
Util::ResourceManager<ResourceType, Identifier>::~ResourceManager()
{

}

template<typename ResourceType, typename Identifier>
void Util::ResourceManager<ResourceType, Identifier>::add(const Identifier& id, const std::string& file)
{
    Resource<ResourceType, Identifier>* res = new Resource<ResourceType, Identifier>(id, *this);
    res->mStrategy = new FileLoadingStrategy<ResourceType>(file);

    mResources[id] = res;
}

template<typename ResourceType, typename Identifier>
template<typename... Args>
void Util::ResourceManager<ResourceType, Identifier>::add(const Identifier& id, const std::string& file, Args... args)
{
    Resource<ResourceType, Identifier>* res = new Resource<ResourceType, Identifier>(id, *this);
    res->mStrategy = new FileLoadingStrategy<ResourceType, Args...>(file, args...);

    mResources[id] = res;
}
template<typename ResourceType, typename Identifier>
void Util::ResourceManager<ResourceType, Identifier>::remove(const Identifier& id)
{
    if (mResources.count(id) == 0)
        return;

    auto obj = mResources[id];
    delete obj;

    mResources.erase(id);
}
template<typename ResourceType, typename Identifier>
bool Util::ResourceManager<ResourceType, Identifier>::has(const Identifier& id) const
{
    return mResources.count(id) > 0;
}

template<typename ResourceType, typename Identifier>
Util::Resource<ResourceType, Identifier>* Util::ResourceManager<ResourceType, Identifier>::get(const Identifier& id)
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