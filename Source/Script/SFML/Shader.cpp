#include "Extensions.hpp"
#include "Extensions.inl"
#include <Script/ScriptExtensions.hpp>
#include <Util/ResourceManager.hpp>

#include <SFML/Graphics/Shader.hpp>

#include <angelscript.h>

#include <cassert>

namespace
{
    typedef Util::Resource<sf::Shader, std::string> Shader_t;
    typedef Util::Resource<sf::Texture, std::string> Texture_t;
    
    void setParameter(const std::string& string, float param, Shader_t* shader)
    {
        (*shader)->setParameter(string, param);
    }
    void setParameter(const std::string& string, float param, float param2, Shader_t* shader)
    {
        (*shader)->setParameter(string, param, param2);
    }
    void setParameter(const std::string& string, float param, float param2, float param3, Shader_t* shader)
    {
        (*shader)->setParameter(string, param, param2, param3);
    }
    void setParameter(const std::string& string, float param, float param2, float param3, float param4, Shader_t* shader)
    {
        (*shader)->setParameter(string, param, param2, param3, param4);
    }
    void setParameter(const std::string& string, const Math::Vector2& vec, Shader_t* shader)
    {
        (*shader)->setParameter(string, vec);
    }
    void setParameter(const std::string& string, const sf::Color& color, Shader_t* shader)
    {
        (*shader)->setParameter(string, color);
    }
    void setParameter(const std::string& string, Texture_t* texture, Shader_t* shader)
    {
        (*shader)->setParameter(string, **texture);
    }
    void setParameterCurrentTexture(const std::string& string, Shader_t* shader)
    {
        (*shader)->setParameter(string, sf::Shader::CurrentTexture);
    }

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;

            r = eng->SetDefaultNamespace("Shaders"); assert(r >= 0);
            r = eng->RegisterGlobalFunction("bool get_Available()", asFUNCTION(sf::Shader::isAvailable), asCALL_CDECL); assert(r >= 0);
            r = eng->SetDefaultNamespace(""); assert(r >= 0);

            r = eng->RegisterObjectType("Shader", 0, asOBJ_REF); assert(r >= 0);
            
            r = eng->RegisterObjectBehaviour("Shader", asBEHAVE_ADDREF, "void f()", asMETHOD(Shader_t, GCAddRef), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Shader", asBEHAVE_RELEASE, "void f()", asMETHOD(Shader_t, GCRelease), asCALL_THISCALL); assert(r >= 0);

            // r = eng->RegisterObjectMethod("Shader", "string get_ID()", asMETHOD(Shader_t, getId), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Shader", "void SetParameter(string&in,float)", asFUNCTIONPR(setParameter, (const std::string&, float, Shader_t*), void), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Shader", "void SetParameter(string&in,float,float)", asFUNCTIONPR(setParameter, (const std::string&, float, float, Shader_t*), void), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Shader", "void SetParameter(string&in,float,float,float)", asFUNCTIONPR(setParameter, (const std::string&, float, float, float, Shader_t*), void), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Shader", "void SetParameter(string&in,float,float,float,float)", asFUNCTIONPR(setParameter, (const std::string&, float, float, float, float, Shader_t*), void), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Shader", "void SetParameter(string&in,Vec2&in)", asFUNCTIONPR(setParameter, (const std::string&, const Math::Vector2&, Shader_t*), void), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Shader", "void SetParameter(string&in,Color&in)", asFUNCTIONPR(setParameter, (const std::string&, const sf::Color&, Shader_t*), void), asCALL_CDECL_OBJLAST); assert(r >= 0);
            // Warning, make sure to keep the reference alive by storing the handle
            r = eng->RegisterObjectMethod("Shader", "void SetParameter(string&in,Texture@)", asFUNCTIONPR(setParameter, (const std::string&, Texture_t*, Shader_t*), void), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->RegisterObjectMethod("Shader", "void SetParameterCurrentTexture(string&in)", asFUNCTION(setParameterCurrentTexture), asCALL_CDECL_OBJLAST); assert(r >= 0);
            
        }, 1);

        return true;
    }
}

bool Script::SFML::Extensions::Shader = Reg();
