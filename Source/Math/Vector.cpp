#include "Vector.hpp"
#include "Common.hpp"

#include <Script/ScriptExtensions.hpp>
#include <angelscript.h>

#include <memory>

#include <cassert>

using namespace Math;


Vector2::Vector2() :
    X(0), Y(0)
{
    
}
Vector2::Vector2(float x, float y) :
    X(x), Y(y)
{

}

bool Vector2::operator==(const Vector2& rhs) const
{
    return FloatCompare(X, rhs.X, 0.0000001f) && FloatCompare(Y, rhs.Y, 0.0000001f);
}

Vector2& Vector2::operator=(Vector2 rhs)
{
    std::swap(X, rhs.X);
    std::swap(Y, rhs.Y);

    return *this;
}

Vector2& Vector2::operator+=(const Vector2& rhs)
{
    X += rhs.X;
    Y += rhs.Y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& rhs)
{
    X -= rhs.X;
    Y -= rhs.Y;
    return *this;
}

Vector2& Vector2::operator*=(const Vector2& rhs)
{
    X *= rhs.X;
    Y *= rhs.Y;
    return *this;
}
Vector2& Vector2::operator*=(float val)
{
    X *= val;
    Y *= val;
    return *this;
}
Vector2& Vector2::operator/=(const Vector2& rhs)
{
    X /= rhs.X;
    Y /= rhs.Y;
    return *this;
}
Vector2& Vector2::operator/=(float val)
{
    X /= val;
    Y /= val;
    return *this;
}

Vector2 Vector2::operator+(const Vector2& rhs) const
{
    return Vector2(X + rhs.X, Y + rhs.Y);
}
Vector2 Vector2::operator-(const Vector2& rhs) const
{
    return Vector2(X - rhs.X, Y - rhs.Y);
}
Vector2 Vector2::operator*(const Vector2& rhs) const
{
    return Vector2(X * rhs.X, Y * rhs.Y);
}
Vector2 Vector2::operator*(float val) const
{
    return Vector2(X * val, Y * val);
}
Vector2 Vector2::operator/(const Vector2& rhs) const
{
    return Vector2(X / rhs.X, Y / rhs.Y);
}
Vector2 Vector2::operator/(float val) const
{
    return Vector2(X / val, Y / val);
}

float Vector2::dotProduct(const Vector2& other) const
{
    return X * other.X + Y * other.Y;
}

void Vector2::normalize()
{
    float len = sqrt(X * X + Y * Y);

    X /= len;
    Y /= len;
}
Vector2 Vector2::getNormalized() const
{
    float len = sqrt(X * X + Y * Y);
    return *this / len;
}

void Vector2::setLength(float len)
{
    float oldLen = sqrt(X * X + Y * Y);

    if (oldLen == 0)
        return;

    float newLen = len / oldLen;
    X *= newLen;
    Y *= newLen;
}
float Vector2::getLength() const
{
    return sqrt(X * X + Y * Y);
}
float Vector2::getLengthSquared() const
{
    return X * X + Y * Y;
}

float Vector2::getDistance(const Vector2& other) const
{
    float dX = other.X - X, dY = other.Y - Y;
    return sqrt(dX * dX + dY * dY);
}
float Vector2::getDistanceSquared(const Vector2& other) const
{
    float dX = other.X - X, dY = other.Y - Y;
    return dX * dX + dY * dY;
}

void Vector2::setAngle(float ang)
{
    float len = sqrt(X * X + Y * Y);
    X = len * cos(ang);
    Y = len * sin(ang);
}
float Vector2::getAngle() const
{
    return atan2(Y, X);
}
float Vector2::getAngleTo(const Vector2& other) const
{
    return atan2(Y - other.Y, X - other.X);
}

void Vector2::rotate(float ang)
{
    float c = cos(ang),
          s = sin(ang),
          nX = c * X - s * Y;

    Y = s * X + c * Y;
    X = nX;
}
Vector2 Vector2::getRotated(float ang) const
{
    float c = cos(ang),
        s = sin(ang);

    return Vector2(c * X - s * Y, s * X + c * Y);
}
Vector2 Vector2::getPerpendicular() const
{
    return Vector2(Y, -X);
}

namespace
{
    void create_Vector2(void* memory) {
        new(memory) Vector2();
    }

    void create_Vector2_val(float x, float y, void* memory) {
        new(memory)Vector2(x, y);
    }

    void destroy_Vector2(Vector2* memory) {
        memory->~Vector2();
    }

    void assign_Vector2(const Vector2& other, Vector2* vec)
    {
        vec->operator=(other);
    }

    void rotate_Vector2(float ang, Vector2* vec)
    {
        vec->rotate(ang * Math::Deg2Rad);
    }
    Vector2 rotated_Vector2(float ang, Vector2* vec)
    {
        return vec->getRotated(ang * Math::Deg2Rad);
    }
    void setAngle_Vector2(float ang, Vector2* vec)
    {
        vec->setAngle(ang * Math::Deg2Rad);
    }
    float getAngle_Vector2(Vector2* vec)
    {
        return vec->getAngle() * Math::Deg2Rad;
    }
    float getAngleTo_Vector2(const Vector2& other,Vector2* vec)
    {
        return vec->getAngleTo(other) * Math::Deg2Rad;
    }

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;
            r = eng->RegisterObjectType("Vec2", sizeof(Vector2), asOBJ_VALUE | asGetTypeTraits<Vector2>()); assert(r >= 0);

            r = eng->RegisterObjectBehaviour("Vec2", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(create_Vector2), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Vec2", asBEHAVE_CONSTRUCT, "void f(float,float)", asFUNCTION(create_Vector2_val), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Vec2", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy_Vector2), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->RegisterObjectMethod("Vec2", "bool opEquals(Vec2&in)", asMETHOD(Vector2, operator==), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "Vec2& opAssign(Vec2&in)", asFUNCTION(assign_Vector2), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->RegisterObjectMethod("Vec2", "Vec2& opAddAssign(Vec2&in)", asMETHOD(Vector2, operator+=), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "Vec2& opSubAssign(Vec2&in)", asMETHOD(Vector2, operator-=), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "Vec2& opMulAssign(Vec2&in)", asMETHODPR(Vector2, operator*=, (const Vector2&), Vector2&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "Vec2& opMulAssign(float)", asMETHODPR(Vector2, operator*=, (float), Vector2&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "Vec2& opDivAssign(Vec2&in)", asMETHODPR(Vector2, operator/=, (const Vector2&), Vector2&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "Vec2& opDivAssign(float)", asMETHODPR(Vector2, operator/=, (float), Vector2&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "Vec2 opAdd(Vec2&in)", asMETHOD(Vector2, operator+), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "Vec2 opSub(Vec2&in)", asMETHOD(Vector2, operator-), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "Vec2 opMul(Vec2&in)", asMETHODPR(Vector2, operator*, (const Vector2&) const, Vector2), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "Vec2 opMul(float)", asMETHODPR(Vector2, operator*, (float) const, Vector2), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "Vec2 opDiv(Vec2&in)", asMETHODPR(Vector2, operator/, (const Vector2&) const, Vector2), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "Vec2 opDiv(float)", asMETHODPR(Vector2, operator/, (float) const, Vector2), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Vec2", "float Dot(Vec2&in)", asMETHOD(Vector2, dotProduct), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "void Normalize()", asMETHOD(Vector2, normalize), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "Vec2 get_Normalized()", asMETHOD(Vector2, getNormalized), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "void set_Length(float)", asMETHOD(Vector2, setLength), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "float get_Length()", asMETHOD(Vector2, getLength), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "float get_LengthSquared()", asMETHOD(Vector2, getLengthSquared), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "float Distance(Vec2&in)", asMETHOD(Vector2, getDistance), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "float DistanceSquared(Vec2&in)", asMETHOD(Vector2, getDistanceSquared), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "void set_Angle(float)", asFUNCTION(setAngle_Vector2), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "float get_Angle()", asFUNCTION(getAngle_Vector2), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "float AngleTo(Vec2&in)", asFUNCTION(getAngleTo_Vector2), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "void Rotate(float)", asFUNCTION(rotate_Vector2), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "Vec2 Rotated(float)", asFUNCTION(rotated_Vector2), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Vec2", "Vec2 get_Perpendicular()", asMETHOD(Vector2, getPerpendicular), asCALL_THISCALL); assert(r >= 0);
        }, -501);

        return true;
    }
}

bool Script::ScriptExtensions::Vector2 = Reg();
