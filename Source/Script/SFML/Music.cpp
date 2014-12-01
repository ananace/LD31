#include "Extensions.hpp"
#include "Extensions.inl"
#include <Script/ScriptExtensions.hpp>
#include <Util/ResourceManager.hpp>

#include <SFML/Audio/Music.hpp>

#include <angelscript.h>

#include <cassert>

namespace
{
    typedef Util::Resource<sf::Music, std::string> Music_t;

    sf::Music::Status getStatus(Music_t* music)
    {
        return (*music)->getStatus();
    }
    bool getLoop(Music_t* music)
    {
        return (*music)->getLoop();
    }
    void setLoop(bool loop, Music_t* music)
    {
        (*music)->setLoop(loop);
    }
    float getPitch(Music_t* music)
    {
        return (*music)->getPitch();
    }
    void setPitch(float pitch, Music_t* music)
    {
        (*music)->setPitch(pitch);
    }
    float getVolume(Music_t* music)
    {
        return (*music)->getVolume();
    }
    void setVolume(float vol, Music_t* music)
    {
        (*music)->setVolume(vol);
    }

    void pause(Music_t* music)
    {
        (*music)->pause();
    }
    void play(Music_t* music)
    {
        (*music)->play();
    }
    void stop(Music_t* music)
    {
        (*music)->stop();
    }


    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;

            r = eng->SetDefaultNamespace("Music"); assert(r >= 0);

            r = eng->RegisterEnum("Status"); assert(r >= 0);
            r = eng->RegisterEnumValue("Status", "Stopped", sf::Music::Stopped); assert(r >= 0);
            r = eng->RegisterEnumValue("Status", "Paused", sf::Music::Paused); assert(r >= 0);
            r = eng->RegisterEnumValue("Status", "Playing", sf::Music::Playing); assert(r >= 0);

            r = eng->SetDefaultNamespace(""); assert(r >= 0);

            r = eng->RegisterObjectType("Music", 0, asOBJ_REF); assert(r >= 0);

            r = eng->RegisterObjectBehaviour("Music", asBEHAVE_ADDREF, "void f()", asMETHOD(Music_t, GCAddRef), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Music", asBEHAVE_RELEASE, "void f()", asMETHOD(Music_t, GCRelease), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Music", "bool get_Loops()", asFUNCTION(getLoop), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Music", "void set_Loops(bool)", asFUNCTION(setLoop), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Music", "float get_Pitch()", asFUNCTION(getPitch), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Music", "void set_Pitch(float)", asFUNCTION(setPitch), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Music", "Music::Status get_Status()", asFUNCTION(getStatus), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Music", "float get_Volume()", asFUNCTION(getVolume), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Music", "void set_Volume(float)", asFUNCTION(setVolume), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->RegisterObjectMethod("Music", "void Pause()", asFUNCTION(pause), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Music", "void Play()", asFUNCTION(play), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Music", "void Stop()", asFUNCTION(stop), asCALL_CDECL_OBJLAST); assert(r >= 0);
        });

        return true;
    }
}

bool Script::SFML::Extensions::Music = Reg();
