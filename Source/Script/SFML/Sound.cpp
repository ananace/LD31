#include "Extensions.hpp"
#include "Extensions.inl"
#include <Math/Rect.hpp>
#include <Math/Vector.hpp>
#include <Script/ScriptExtensions.hpp>
#include <Util/ResourceManager.hpp>

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <angelscript.h>

#include <cassert>

namespace
{
    typedef Util::Resource<sf::SoundBuffer, std::string> SoundBuffer_t;

    std::unordered_map<void*, SoundBuffer_t*> sounds;

    void create_sound(void* memory)
    {
        new(memory)sf::Sound();
    }
    void create_sound_buffer(SoundBuffer_t* buffer, void* memory)
    {
        buffer->GCAddRef();
        new(memory)sf::Sound(**buffer);

        sounds[memory] = buffer;
    }
    void destruct_sound(sf::Sound* sound)
    {
        sound->~Sound();

        sounds[sound]->GCRelease();
        sounds.erase(sound);
    }

    SoundBuffer_t* getBuffer(sf::Sound& sound)
    {
        return sounds[&sound];
    }
    void setBuffer(SoundBuffer_t* buffer, sf::Sound& sound)
    {
        buffer->GCAddRef();
        sounds[&sound]->GCRelease();

        sound.setBuffer(**buffer);

        sounds[&sound] = buffer;
    }


    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;

            r = eng->SetDefaultNamespace("Sound"); assert(r >= 0);

            r = eng->RegisterObjectType("Buffer", 0, asOBJ_REF | asOBJ_NOCOUNT); assert(r >= 0);

            r = eng->RegisterObjectMethod("Buffer", "string get_ID()", asMETHOD(SoundBuffer_t, getId), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterEnum("Status"); assert(r >= 0);
            r = eng->RegisterEnumValue("Status", "Stopped", sf::Sound::Stopped); assert(r >= 0);
            r = eng->RegisterEnumValue("Status", "Paused", sf::Sound::Paused); assert(r >= 0);
            r = eng->RegisterEnumValue("Status", "Playing", sf::Sound::Playing); assert(r >= 0);

            r = eng->SetDefaultNamespace(""); assert(r >= 0);

            r = eng->RegisterObjectType("Sound", sizeof(sf::Sound), asOBJ_VALUE | asGetTypeTraits<sf::Sound>()); assert(r >= 0);
            
            r = eng->RegisterObjectBehaviour("Sound", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(create_sound), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Sound", asBEHAVE_CONSTRUCT, "void f(Sound::Buffer@)", asFUNCTION(create_sound_buffer), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Sound", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destruct_sound), asCALL_CDECL_OBJLAST); assert(r >= 0);
            
            r = eng->RegisterObjectMethod("Sound", "bool get_Loops()", asMETHOD(sf::Sound, getLoop), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Sound", "void set_Loops(bool)", asMETHOD(sf::Sound, setLoop), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Sound", "Sound::Status get_Status()", asMETHOD(sf::Sound, getStatus), asCALL_THISCALL); assert(r >= 0);
            ///\TODO: Time values, duration / playing offset

            r = eng->RegisterObjectMethod("Sound", "Sound::Buffer@ GetBuffer()", asFUNCTION(getBuffer), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Sound", "void SetBuffer(Sound::Buffer@)", asFUNCTION(setBuffer), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Sound", "void Play()", asMETHOD(sf::Sound, play), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Sound", "void Pause()", asMETHOD(sf::Sound, pause), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Sound", "void Stop()", asMETHOD(sf::Sound, stop), asCALL_THISCALL); assert(r >= 0);

            Script::SFML::registerSoundSource<sf::Sound>("Sound", eng);
        });

        return true;
    }
}

bool Script::SFML::Extensions::Sound = Reg();
