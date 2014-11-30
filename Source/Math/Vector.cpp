#include "Vector.hpp"
#include "Common.hpp"

#include <memory>

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
