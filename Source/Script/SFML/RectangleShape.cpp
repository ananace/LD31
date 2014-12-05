#include "Extensions.hpp"
#include "Extensions.inl"
#include <Math/Rect.hpp>
#include <Math/Vector.hpp>
#include <Script/ScriptExtensions.hpp>

#include <SFML/Graphics/RectangleShape.hpp>

#include <angelscript.h>

#include <cassert>

namespace
{
    void create_RectangleShape(void* memory)
    {
        new(memory)sf::RectangleShape();
    }
    void create_RectangleShape_rect(const Math::Rect& rect, void* memory)
    {
        new(memory)sf::RectangleShape(rect.getSize());
        ((sf::RectangleShape*)memory)->setPosition(rect.getTopLeft());
    }
    void create_RectangleShape_vec(const Math::Vector2& vec, void* memory)
    {
        new(memory)sf::RectangleShape(vec);
    }

    Math::Rect getRect(sf::RectangleShape& shape)
    {
        auto origin = shape.getOrigin();
        auto pos = shape.getPosition();
        auto size = shape.getSize();

        return Math::Rect(pos - origin, pos - origin + size);
    }
    void setRect(const Math::Rect& rect, sf::RectangleShape& shape)
    {
        shape.setOrigin(0, 0);
        shape.setSize(rect.getSize());
        shape.setPosition(rect.getTopLeft());
    }
    Math::Vector2 getSize(sf::RectangleShape& shape)
    {
        return shape.getSize();
    }
    void setSize(const Math::Vector2& vec, sf::RectangleShape& shape)
    {
        shape.setSize(vec);
    }
    Math::Rect getGlobalBounds(sf::RectangleShape& shape)
    {
        return shape.getGlobalBounds();
    }
    Math::Rect getLocalBounds(sf::RectangleShape& shape)
    {
        return shape.getLocalBounds();
    }

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;

            r = eng->SetDefaultNamespace("Shapes"); assert(r >= 0);

            r = eng->RegisterObjectType("Rectangle", sizeof(sf::RectangleShape), asOBJ_VALUE | asGetTypeTraits<sf::RectangleShape>()); assert(r >= 0);

            r = eng->RegisterObjectBehaviour("Rectangle", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(create_RectangleShape), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Rectangle", asBEHAVE_CONSTRUCT, "void f(Rect&in)", asFUNCTION(create_RectangleShape_rect), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Rectangle", asBEHAVE_CONSTRUCT, "void f(Vec2&in)", asFUNCTION(create_RectangleShape_vec), asCALL_CDECL_OBJLAST); assert(r >= 0);

            Script::SFML::registerShape<sf::RectangleShape>("Rectangle", eng);

            r = eng->RegisterObjectMethod("Rectangle", "Rect get_Rect()", asFUNCTION(getRect), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Rectangle", "void set_Rect(Rect&in)", asFUNCTION(setRect), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Rectangle", "Vec2 get_Size()", asFUNCTION(getRect), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Rectangle", "void set_Size(Vec2&in)", asFUNCTION(setRect), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->SetDefaultNamespace(""); assert(r >= 0);
        }, 6);

        return true;
    }
}

bool Script::SFML::Extensions::RectangleShape = Reg();
