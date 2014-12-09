#include "Extensions.hpp"
#include <Math/Vector.hpp>
#include <Script/ScriptExtensions.hpp>
#include <Util/ResourceManager.hpp>

#include <SFML/Graphics/Texture.hpp>

#include <angelscript.h>

typedef Util::Resource<sf::Texture, std::string> Texture_t;

namespace
{
#ifdef AS_SUPPORT_VALRET
    Math::Vector2 getSize(Texture_t* texture)
    {
        return (*texture)->getSize();
    }
#else
    void getSize(asIScriptGeneric* gen)
    {
        Texture_t*  obj = reinterpret_cast<Texture_t*>(gen->GetObject());
        new(gen->GetAddressOfReturnLocation()) Math::Vector2((*obj)->getSize());
    }
#endif

    bool isSmooth(Texture_t* texture)
    {
        return (*texture)->isSmooth();
    }
    void setSmooth(bool smooth, Texture_t* texture)
    {
        (*texture)->setSmooth(smooth);
    }
    bool isRepeated(Texture_t* texture)
    {
        return (*texture)->isRepeated();
    }
    void setRepeated(bool repeat, Texture_t* texture)
    {
        (*texture)->setRepeated(repeat);
    }
}

namespace
{

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;

            r = eng->RegisterObjectType("Texture", 0, asOBJ_REF | asOBJ_NOCOUNT); assert(r >= 0);

            r = eng->RegisterObjectMethod("Texture", "string get_ID()", asMETHOD(Texture_t, getId), asCALL_THISCALL); assert(r >= 0);

#ifdef AS_SUPPORT_VALRET
            r = eng->RegisterObjectMethod("Texture", "Vec2 get_Size()", asFUNCTION(getSize), asCALL_CDECL_OBJLAST); assert(r >= 0);
#else
            r = eng->RegisterObjectMethod("Texture", "Vec2 get_Size()", asFUNCTION(getSize), asCALL_GENERIC); assert(r >= 0);
#endif
            r = eng->RegisterObjectMethod("Texture", "bool get_Smooth()", asFUNCTION(isSmooth), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Texture", "void set_Smooth(bool)", asFUNCTION(setSmooth), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Texture", "bool get_Repeated()", asFUNCTION(isRepeated), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Texture", "void set_Repeated(bool)", asFUNCTION(setRepeated), asCALL_CDECL_OBJLAST); assert(r >= 0);
        });

        return true;
    }

}

bool Script::SFML::Extensions::Texture = Reg();
