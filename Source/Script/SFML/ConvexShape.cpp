#include "Extensions.hpp"
#include "Extensions.inl"
#include <Math/Rect.hpp>
#include <Math/Vector.hpp>
#include <Script/ScriptExtensions.hpp>

#include <SFML/Graphics/ConvexShape.hpp>

#include <angelscript.h>

#include <cassert>

namespace
{
    void create_ConvexShape(void* memory)
    {
        new(memory)sf::ConvexShape();
    }
    void create_ConvexShape_count(unsigned int count, void* memory)
    {
        new(memory)sf::ConvexShape(count);
    }
    void create_ConvexShape_list(void* list, sf::ConvexShape* memory)
    {
        uint32_t count = *(asUINT*)list;

        new(memory)sf::ConvexShape(count);

        Math::Vector2* veclist = (Math::Vector2*)(((asUINT*)list) + 1);

        for (uint32_t i = 0; i < count; ++i)
        {
            memory->setPoint(i, *veclist++);
        }
    }

    void setPoint(unsigned int index, const Math::Vector2& vec, sf::ConvexShape& shape)
    {
        shape.setPoint(index, vec);
    }

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;

            r = eng->SetDefaultNamespace("Shapes"); assert(r >= 0);

            r = eng->RegisterObjectType("Polygon", sizeof(sf::ConvexShape), asOBJ_VALUE | asGetTypeTraits<sf::ConvexShape>()); assert(r >= 0);

            r = eng->RegisterObjectBehaviour("Polygon", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(create_ConvexShape), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Polygon", asBEHAVE_CONSTRUCT, "void f(uint)", asFUNCTION(create_ConvexShape_count), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Polygon", asBEHAVE_LIST_CONSTRUCT, "void f(int&in) {repeat Vec2}", asFUNCTION(create_ConvexShape_list), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->RegisterObjectMethod("Polygon", "void set_array(uint,Vec2&in)", asFUNCTION(setPoint), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Polygon", "void set_PointCount(uint)", asMETHOD(sf::ConvexShape, setPointCount), asCALL_THISCALL); assert(r >= 0);

            Script::SFML::registerShape<sf::ConvexShape>("Polygon", eng);

            r = eng->SetDefaultNamespace(""); assert(r >= 0);
        }, 1);

        return true;
    }
}

bool Script::SFML::Extensions::ConvexShape = Reg();
