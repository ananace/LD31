#include "Rect.hpp"
#include "Common.hpp"

#include <Script/ScriptExtensions.hpp>
#include <angelscript.h>

#include <cassert>

using namespace Math;

Rect::Rect() :
    Top(0), Left(0), Width(0), Height(0)
{
}
Rect::Rect(float left, float top, float width, float height)
{
    if (width < 0)
    {
        left = left - width;
        width = -width;
    }
    if (height < 0)
    {
        top = top - height;
        height = -height;
    }

    Top = top;
    Left = left;
    Width = width;
    Height = height;
}
Rect::Rect(const Vector2& topleft, float width, float height)
{
    float top = topleft.Y, left = topleft.X;

    if (width < 0)
    {
        left = left - width;
        width = -width;
    }
    if (height < 0)
    {
        top = top - height;
        height = -height;
    }

    Top = top;
    Left = left;
    Width = width;
    Height = height;
}
Rect::Rect(const Vector2& topleft, const Vector2& bottomright) :
    Top(topleft.Y), Left(topleft.X), Width(bottomright.X - topleft.X), Height(bottomright.Y - topleft.Y)
{
}

bool Rect::operator==(const Rect& rhs) const
{
    return FloatCompare(Top, rhs.Top, 0.0000001f) && FloatCompare(Left, rhs.Left, 0.0000001f) && FloatCompare(Width, rhs.Width, 0.0000001f) && FloatCompare(Height, rhs.Height, 0.0000001f);
}

Vector2 Rect::getTopLeft() const
{
    return Vector2(Left, Top);
}
Vector2 Rect::getBottomRight() const
{
    return Vector2(Left + Width, Top + Height);
}
Vector2 Rect::getCenter() const
{
    return Vector2(Left + Width / 2.f, Top + Height / 2.f);
}
Vector2 Rect::getSize() const
{
    return Vector2(Width, Height);
}

Vector2 Rect::constrain(const Vector2& point)
{
    return Vector2(point.X < Left ? Left : (point.X > Left + Width ? Left + Width : point.X), point.Y < Top ? Top : (point.Y > Top + Height ? Top + Height : point.Y));
}

bool Rect::contains(const Vector2& point)
{
    return Left <= point.X && Top <= point.Y && Left + Width >= point.X && Top + Height >= point.Y;
}
bool Rect::intersects(const Rect& rect)
{
    float interLeft = std::max(Left, rect.Left),
        interTop = std::max(Top, rect.Top),
        interRight = std::max(Left + Width, rect.Left + rect.Width),
        interBottom = std::max(Top + Height, rect.Top + rect.Height);

    return (interLeft < interRight && interTop < interBottom);
}
bool Rect::intersects(const Rect& rect, Rect& intersecting)
{
    float interLeft = std::max(Left, rect.Left),
        interTop = std::max(Top, rect.Top),
        interRight = std::max(Left + Width, rect.Left + rect.Width),
        interBottom = std::max(Top + Height, rect.Top + rect.Height);

    if (interLeft < interRight && interTop < interBottom)
    {
        intersecting = Rect(interLeft, interTop, interRight - interLeft, interBottom - interTop);
        return true;
    }

    intersecting = Rect();
    return false;
}

namespace
{
    void create_rect(void* memory) {
        new(memory) Rect();
    }
    void create_rect_loose(float l, float t, float w, float h, void* memory) {
        new(memory) Rect(l, t, w, h);
    }
    void create_rect_tlwh(const Vector2& tl, float w, float h, void* memory) {
        new(memory) Rect(tl, w, h);
    }
    void create_rect_tlbr(const Vector2& tl, const Vector2& br, void* memory) {
        new(memory) Rect(tl, br);
    }
    void destruct_rect(Rect* memory) {
        memory->~Rect();
    }

    void rect_setBottomRight(const Vector2& br, Rect& rect)
    {
        auto tl = rect.getTopLeft();
        if (tl.X > br.X || tl.Y > br.Y)
            return;

        rect.Width = br.X - tl.X;
        rect.Height = br.Y - tl.Y;
    }

    void rect_setTopLeft(const Vector2& tl, Rect& rect)
    {
        auto br = rect.getBottomRight();
        if (tl.X > br.X || tl.Y > br.Y)
            return;

        rect.Top = tl.Y;
        rect.Left = tl.X;
        rect.Width = br.X - tl.X;
        rect.Height = br.Y - tl.Y;
    }

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;
            r = eng->RegisterObjectType("Rect", sizeof(Rect), asOBJ_VALUE | asGetTypeTraits<Rect>()); assert(r >= 0);

            r = eng->RegisterObjectBehaviour("Rect", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(create_rect), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Rect", asBEHAVE_CONSTRUCT, "void f(float,float,float,float)", asFUNCTION(create_rect_loose), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Rect", asBEHAVE_CONSTRUCT, "void f(Vec2,float,float)", asFUNCTION(create_rect_tlwh), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Rect", asBEHAVE_CONSTRUCT, "void f(Vec2,Vec2)", asFUNCTION(create_rect_tlbr), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Rect", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destruct_rect), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->RegisterObjectMethod("Rect", "Rect& opAssign(Rect)", asMETHOD(Rect, operator=), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Rect", "bool opEquals(Rect&in)", asMETHOD(Rect, operator==), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectProperty("Rect", "float Top", asOFFSET(Rect, Top)); assert(r >= 0);
            r = eng->RegisterObjectProperty("Rect", "float Left", asOFFSET(Rect, Left)); assert(r >= 0);
            r = eng->RegisterObjectProperty("Rect", "float Width", asOFFSET(Rect, Width)); assert(r >= 0);
            r = eng->RegisterObjectProperty("Rect", "float Height", asOFFSET(Rect, Height)); assert(r >= 0);

            r = eng->RegisterObjectMethod("Rect", "Vec2 get_Center()", asMETHOD(Rect, getCenter), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Rect", "Vec2 get_TopLeft()", asMETHOD(Rect, getTopLeft), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Rect", "void set_TopLeft(Vec2&in)", asFUNCTION(rect_setTopLeft), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Rect", "Vec2 get_BottomRight()", asMETHOD(Rect, getBottomRight), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Rect", "void set_BottomRight(Vec2&in)", asFUNCTION(rect_setBottomRight), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Rect", "Vec2 get_Size()", asMETHOD(Rect, getSize), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Rect", "bool contains(Vec2&in)", asMETHOD(Rect, contains), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Rect", "bool intersects(Rect&in)", asMETHODPR(Rect, intersects, (const Rect&), bool), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Rect", "bool intersects(Rect&in, Rect&out)", asMETHODPR(Rect, intersects, (const Rect&, Rect&), bool), asCALL_THISCALL); assert(r >= 0);
        }, -500);

        return true;
    }
}

bool Script::ScriptExtensions::Rect = Reg();
