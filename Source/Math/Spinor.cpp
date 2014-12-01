#include "Spinor.hpp"
#include "Common.hpp"

#include <memory>
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

/*
void Spinor::setLength(float len)
{
	
}
*/
float Spinor::getLength() const
{
    return sqrt(Real * Real + Complex * Complex);
}
/*
void Spinor::setLengthSquared(float len)
{
	
}
*/
float Spinor::getLengthSquared() const
{
    return Real * Real + Complex * Complex;
}
/*
void Spinor::setAngle(float ang)
{
	
}
*/
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

    if (1 - cosom > 0.001)
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
