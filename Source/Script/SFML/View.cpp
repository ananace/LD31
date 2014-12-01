#include "Extensions.hpp"
#include <Script/ScriptExtensions.hpp>
#include <Math/Common.hpp>
#include <Math/Rect.hpp>
#include <Math/Vector.hpp>

#include <SFML/Graphics/View.hpp>

#include <angelscript.h>

#include <cassert>

Math::Vector2 getCenter(sf::View* view)
{
    return view->getCenter();
}
void setCenter(const Math::Vector2& vec, sf::View* view)
{
    view->setCenter(vec);
}
Math::Vector2 getSize(sf::View* view)
{
    return view->getSize();
}
void setSize(const Math::Vector2& vec, sf::View* view)
{
    view->setSize(vec);
}
Math::Rect getViewport(sf::View* view)
{
    return view->getViewport();
}
void setViewport(const Math::Rect& rect, sf::View* view)
{
    view->setViewport(rect);
}
void move(const Math::Vector2& vec, sf::View* view)
{
    view->move(vec);
}
void reset(const Math::Rect& rect, sf::View* view)
{
    view->reset(rect);
}

namespace
{
    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng){
            int r = 0;

            r = eng->RegisterObjectType("View", 0, asOBJ_REF | asOBJ_NOCOUNT); assert(r >= 0);

            r = eng->RegisterObjectMethod("View", "Vec2 get_Center()", asFUNCTION(getCenter), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("View", "void set_Center(Vec2&in)", asFUNCTION(setCenter), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("View", "void SetCenter(float,float)", asMETHODPR(sf::View, setCenter, (float, float), void), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("View", "Vec2 get_Size()", asFUNCTION(getSize), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("View", "void set_Size(Vec2&in)", asFUNCTION(setSize), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("View", "void SetSize(float,float)", asMETHODPR(sf::View, setSize, (float, float), void), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("View", "float get_Rotation()", asMETHOD(sf::View, getRotation), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("View", "void set_Rotation(float)", asMETHOD(sf::View, setRotation), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("View", "Rect get_Viewport()", asFUNCTION(getViewport), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("View", "void set_Viewport(Rect&in)", asFUNCTION(setViewport), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->RegisterObjectMethod("View", "void Move(float,float)", asMETHODPR(sf::View, move, (float, float), void), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("View", "void Move(Vec2&in)", asFUNCTION(move), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("View", "void Rotate(float)", asMETHOD(sf::View, rotate), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("View", "void Reset(Rect&in)", asFUNCTION(reset), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("View", "void Zoom(float)", asMETHOD(sf::View, zoom), asCALL_THISCALL); assert(r >= 0);
        });

        return true;
    }
}

bool Script::SFML::Extensions::View = Reg();
