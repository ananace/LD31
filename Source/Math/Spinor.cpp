#include "Spinor.hpp"
#include "Common.hpp"
#include <Script/ScriptExtensions.hpp>

#include <angelscript.h>

#include <memory>
#include <cassert>
#include <cmath>

using namespace Math;

Spinor::Spinor() :
    Real(0), Complex(0)
{
	
}
Spinor::Spinor(float ang) :
    Real(cos(ang)), Complex(sin(ang))
{
	
}
Spinor::Spinor(float real, float complex) :
    Real(real), Complex(complex)
{
	
}

bool Spinor::operator==(const Spinor& rhs)
{
    return FloatCompare(Real, rhs.Real, 0.0000001f) && FloatCompare(Complex, rhs.Complex, 0.0000001f);
}

Spinor& Spinor::operator=(Spinor other)
{
    std::swap(Real, other.Real);
    std::swap(Complex, other.Complex);

    return *this;
}

Spinor& Spinor::operator+=(const Spinor& rhs)
{
    Real += rhs.Real;
    Complex += rhs.Complex;
    return *this;
}
Spinor& Spinor::operator-=(const Spinor& rhs)
{
    Real -= rhs.Real;
    Complex -= rhs.Complex;
    return *this;
}
Spinor& Spinor::operator*=(const Spinor& rhs)
{
    Real = Real * rhs.Real - Complex * rhs.Complex;
    Complex = Real * rhs.Complex + Complex * rhs.Real;
    return *this;
}
Spinor& Spinor::operator*=(float val)
{
    Real *= val;
    Complex *= val;
    return *this;
}
/*
Spinor& Spinor::operator/=(const Spinor& rhs)
{
	
}
*/
Spinor& Spinor::operator/=(float val)
{
    Real /= val;
    Complex /= val;
    return *this;
}

Spinor Spinor::operator+(const Spinor& rhs) const
{
    return Spinor(Real + rhs.Real, Complex + rhs.Complex);
}
Spinor Spinor::operator-(const Spinor& rhs) const
{
    return Spinor(Real - rhs.Real, Complex - rhs.Complex);
}
Spinor Spinor::operator*(const Spinor& rhs) const
{
    return Spinor(Real * rhs.Real - Complex * rhs.Complex, Real * rhs.Complex + Complex * rhs.Real);
}
Spinor Spinor::operator*(float val) const
{
    return Spinor(Real * val, Complex * val);
}
/*
Spinor Spinor::operator/(const Spinor& rhs) const
{
	
}
*/
Spinor Spinor::operator/(float val) const
{
    return Spinor(Real / val, Complex / val);
}

void Spinor::invert()
{
    float len = sqrt(Real * Real + Complex * Complex);

    Real *= len;
    Complex *= -len;
}
Spinor Spinor::getInverted() const
{
    float len = sqrt(Real * Real + Complex * Complex);

    return Spinor(Real * len, -Complex * len);
}
void Spinor::normalize()
{
    float len = sqrt(Real * Real + Complex * Complex);

    Real /= len;
    Complex /= len;
}
Spinor Spinor::getNormalized() const
{
    float len = sqrt(Real * Real + Complex * Complex);
    return Spinor(Real / len, Complex / len);
}


void Spinor::setLength(float len)
{
    float oldLen = sqrt(Real * Real + Complex * Complex);
    if (oldLen == 0)
        return;

    float newLen = len / oldLen;
    Real *= newLen;
    Complex *= newLen;
}

float Spinor::getLength() const
{
    return sqrt(Real * Real + Complex * Complex);
}

void Spinor::setLengthSquared(float len)
{
    float oldLen = Real * Real + Complex * Complex;
    if (oldLen == 0)
        return;

    float newLen = len / oldLen;
    Real *= newLen;
    Complex *= newLen;
}

float Spinor::getLengthSquared() const
{
    return Real * Real + Complex * Complex;
}

void Spinor::setAngle(float ang)
{
    Real = cos(ang);
    Complex = sin(ang);
}

float Spinor::getAngle() const
{
    return atan2(Complex, Real) * 2;
}

float Spinor::dotProduct(const Spinor& other) const
{
    return Real * other.Real + Complex * other.Complex;
}

Spinor Spinor::slerp(const Spinor& end, float t) const
{
    float tr, tc, omega, cosom, sinom, scale0, scale1;

    cosom = Real * end.Real + Complex * end.Complex;

    if (cosom < 0)
    {
        cosom = -cosom;
        tc = -end.Complex;
        tr = -end.Real;
    }
    else
    {
        tc = end.Complex;
        tr = end.Real;
    }

    if (1 - cosom > 0.001f)
    {
        omega = acos(cosom);
        sinom = sin(omega);
        scale0 = sin((1 - t) * omega) / sinom;
        scale1 = sin(t * omega) / sinom;
    }
    else
    {
        scale0 = 1 - t;
        scale1 = t;
    }

    return Spinor(scale0 * Real + scale1 * tr, scale0 * Complex + scale1 * tc);
}

namespace
{
    void Spinor_assign(const Spinor& rhs, Spinor& lhs)
    {
        lhs = rhs;
    }

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;

            r = eng->RegisterObjectType("Spinor", sizeof(Spinor), asOBJ_VALUE | asGetTypeTraits<Spinor>()); assert(r >= 0);

            r = eng->RegisterObjectMethod("Spinor", "bool opCmp(Spinor&in)", asMETHOD(Spinor, operator==), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Spinor", "Spinor& opAssign(Spinor&in)", asFUNCTION(Spinor_assign), asCALL_CDECL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Spinor", "Spinor& opAddAssign(Spinor&in)", asMETHOD(Spinor, operator+=), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Spinor", "Spinor& opSubAssign(Spinor&in)", asMETHOD(Spinor, operator-=), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Spinor", "Spinor& opMulAssign(Spinor&in)", asMETHODPR(Spinor, operator*=, (const Spinor&), Spinor&), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Spinor", "Spinor& opMulAssign(float)", asMETHODPR(Spinor, operator*=, (float), Spinor&), asCALL_THISCALL); assert(r >= 0);
            // r = eng->RegisterObjectMethod("Spinor", "Spinor& opDivAssign(Spinor&in)", asMETHODPR(Spinor, operator/=, (const Spinor&), Spinor&), asCALL_THISCALL); assert(r >= 0);
            // r = eng->RegisterObjectMethod("Spinor", "Spinor& opDivAssign(float)", asMETHODPR(Spinor, operator/=, (float), Spinor&), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Spinor", "Spinor opAdd(Spinor&in)", asMETHOD(Spinor, operator+), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Spinor", "Spinor opSub(Spinor&in)", asMETHOD(Spinor, operator+), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Spinor", "Spinor opMul(Spinor&in)", asMETHODPR(Spinor, operator*, (const Spinor&) const, Spinor), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Spinor", "Spinor opMul(float)", asMETHODPR(Spinor, operator*, (float) const, Spinor), asCALL_THISCALL); assert(r >= 0);
            // r = eng->RegisterObjectMethod("Spinor", "Spinor opDiv(Spinor&in)", asMETHODPR(Spinor, operator/, (const Spinor&) const, Spinor), asCALL_THISCALL); assert(r >= 0);
            // r = eng->RegisterObjectMethod("Spinor", "Spinor opDiv(float)", asMETHODPR(Spinor, operator/, (float) const, Spinor), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Spinor", "float get_Angle()", asMETHOD(Spinor, getAngle), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Spinor", "void set_Angle(float)", asMETHOD(Spinor, setAngle), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Spinor", "void get_Inverted()", asMETHOD(Spinor, getInverted), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Spinor", "float get_Length()", asMETHOD(Spinor, getLength), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Spinor", "void set_Length(float)", asMETHOD(Spinor, setLength), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Spinor", "float get_LengthSquared()", asMETHOD(Spinor, getLengthSquared), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Spinor", "void set_LengthSquared(float)", asMETHOD(Spinor, setLengthSquared), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Spinor", "void get_Normalized()", asMETHOD(Spinor, getNormalized), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Spinor", "float Dot(Spinor&in)", asMETHOD(Spinor, dotProduct), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Spinor", "void Invert()", asMETHOD(Spinor, invert), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Spinor", "void Normalize()", asMETHOD(Spinor, normalize), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Spinor", "Spinor Slerp(Spinor&in,float)", asMETHOD(Spinor, slerp), asCALL_THISCALL); assert(r >= 0);
        });

        return true;
    }
}

bool Script::ScriptExtensions::Spinor = Reg();
