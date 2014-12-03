#pragma once

#include <SFML/System/Vector2.hpp>

namespace Math
{

struct Vector2
{
    /// Default constructor
    Vector2();
    /// Component constructor
    Vector2(float x, float y);    

    /// SFML conversion constructor
    template<typename T>
    Vector2(const sf::Vector2<T>& vec);

    /// Copy constructor
    Vector2(const Vector2&) = default;
    /// Destructor
    ~Vector2() = default;

    /// SFML conversion casting
    template<typename T>
    operator sf::Vector2<T>() const;

    /// Epsilon comparison
    bool operator==(const Vector2& rhs) const;

    /// Assignment operator
    Vector2& operator=(Vector2 rhs);

    // Arithmetic assignment operators
    Vector2& operator+=(const Vector2& rhs);
    Vector2& operator-=(const Vector2& rhs);
    Vector2& operator*=(const Vector2& rhs);
    Vector2& operator*=(float val);
    Vector2& operator/=(const Vector2& rhs);
    Vector2& operator/=(float val);

    // Arithmetic operators
    Vector2 operator+(const Vector2& rhs) const;
    Vector2 operator-(const Vector2& rhs) const;
    Vector2 operator*(const Vector2& rhs) const;
    Vector2 operator*(float val) const;
    Vector2 operator/(const Vector2& rhs) const;
    Vector2 operator/(float val) const;

    // Useful vector math functions

    float dotProduct(const Vector2& other) const;

    void normalize();
    Vector2 getNormalized() const;

    void setLength(float len);
    float getLength() const;
    float getLengthSquared() const;

    float getDistance(const Vector2& other) const;
    float getDistanceSquared(const Vector2& other) const;

    void setAngle(float ang);
    float getAngle() const;
    float getAngleTo(const Vector2& other) const;

    void rotate(float ang);
    Vector2 getRotated(float ang) const;
    Vector2 getPerpendicular() const;

    // Components

    float X, Y;
};

}

#include "Vector.inl"
