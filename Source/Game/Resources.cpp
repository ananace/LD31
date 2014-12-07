#include "Resources.hpp"

#include <Script/ScriptExtensions.hpp>

#include <angelscript.h>

#include <cassert>

Util::ResourceManager<sf::Music, std::string> Resources::Musics;
Util::ResourceManager<sf::Shader, std::string> Resources::Shaders;
Util::ResourceManager<sf::SoundBuffer, std::string> Resources::Sounds;
Util::ResourceManager<sf::Texture, std::string> Resources::Textures;

namespace
{
    template<typename T>
    Resource<T, std::string>* getResource(const std::string& id);

    template<>
    Resource<sf::Music, std::string>* getResource(const std::string& id)
    {
        if (!Resources::Musics.has(id))
            return nullptr;

        return Resources::Musics.get(id);
    }
    template<>
    Resource<sf::Shader, std::string>* getResource(const std::string& id)
    {
        if (!Resources::Shaders.has(id))
            return nullptr;

        return Resources::Shaders.get(id);
    }
    template<>
    Resource<sf::SoundBuffer, std::string>* getResource(const std::string& id)
    {
        if (!Resources::Sounds.has(id))
            return nullptr;

        return Resources::Sounds.get(id);
    }
    template<>
    Resource<sf::Texture, std::string>* getResource(const std::string& id)
    {
        if (!Resources::Textures.has(id))
            return nullptr;

        return Resources::Textures.get(id);
    }

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;

            r = eng->SetDefaultNamespace("Resources"); assert(r >= 0);

            r = eng->RegisterGlobalFunction("Music@ GetMusic(string&in)", asFUNCTION(getResource<sf::Music>), asCALL_CDECL); assert(r >= 0);
            r = eng->RegisterGlobalFunction("Shader@ GetShader(string&in)", asFUNCTION(getResource<sf::Shader>), asCALL_CDECL); assert(r >= 0);
            r = eng->RegisterGlobalFunction("Sound::Buffer@ GetSound(string&in)", asFUNCTION(getResource<sf::SoundBuffer>), asCALL_CDECL); assert(r >= 0);
            r = eng->RegisterGlobalFunction("Texture@ GetTexture(string&in)", asFUNCTION(getResource<sf::Texture>), asCALL_CDECL); assert(r >= 0);

            r = eng->SetDefaultNamespace(""); assert(r >= 0);
        }, 10);

        return true;
    }

    bool ResMan = Reg();
}


