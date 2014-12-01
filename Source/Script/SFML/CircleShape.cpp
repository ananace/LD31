#include "Extensions.hpp"
#include "Extensions.inl"
#include <Math/Rect.hpp>
#include <Math/Vector.hpp>
#include <Script/ScriptExtensions.hpp>

#include <SFML/Graphics/CircleShape.hpp>

#include <angelscript.h>

#include <cassert>

namespace
{
    void create_CircleShape(void* memory)
    {
        new(memory)sf::CircleShape();
    }
    void create_CircleShape_rad(float radius, unsigned int count, void* memory)
    {
        new(memory)sf::CircleShape(radius, count);
    }

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;

            r = eng->SetDefaultNamespace("Shapes"); assert(r >= 0);

            r = eng->RegisterObjectType("Circle", sizeof(sf::CircleShape), asOBJ_VALUE | asGetTypeTraits<sf::CircleShape>()); assert(r >= 0);

            r = eng->RegisterObjectBehaviour("Circle", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(create_CircleShape), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Circle", asBEHAVE_CONSTRUCT, "void f(float,uint=30)", asFUNCTION(create_CircleShape_rad), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->RegisterObjectMethod("Circle", "void set_PointCount(uint)", asMETHOD(sf::CircleShape, setPointCount), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Circle", "float get_Radius()", asMETHOD(sf::CircleShape, getRadius), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Circle", "void set_Radius(float)", asMETHOD(sf::CircleShape, setRadius), asCALL_THISCALL); assert(r >= 0);

            Script::SFML::registerShape<sf::CircleShape>("Circle", eng);

            r = eng->SetDefaultNamespace(""); assert(r >= 0);
        }, 1);

        return true;
    }
}

bool Script::SFML::Extensions::CircleShape = Reg();
