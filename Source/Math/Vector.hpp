#pragma once

#include <SFML/System/Vector2.hpp>

namespace Math
{

/** \brief A class defining a point or a vector in 2D space
 *
 */
struct Vector2
{
    /// Zeroing constructor
    Vector2();
    /// Component constructor
    Vector2(float x, float y);    

    /// SFML conversion constructor
    template<typename T>
    Vector2(const sf::Vector2<T>& vec);

    /// Default copy constructor
    Vector2(const Vector2&) = default;
    /// Default destructor
    ~Vector2() = default;

    /// SFML conversion casting
    template<typename T>
    operator sf::Vector2<T>() const;

    /// Epsilon comparison
    bool operator==(const Vector2& rhs) const;

    /// Default Assignment operator
    Vector2& operator=(const Vector2& rhs) = default;

    ///\name Arithmetic assignment operators
    ///@{
    Vector2& operator+=(const Vector2& rhs);
    Vector2& operator-=(const Vector2& rhs);
    Vector2& operator*=(const Vector2& rhs);
    Vector2& operator*=(float val);
    Vector2& operator/=(const Vector2& rhs);
    Vector2& operator/=(float val);
    ///@}

    ///\name Arithmetic operators
    ///@{
    Vector2 operator+(const Vector2& rhs) const;
    Vector2 operator-(const Vector2& rhs) const;
    Vector2 operator*(const Vector2& rhs) const;
    Vector2 operator*(float val) const;
    Vector2 operator/(const Vector2& rhs) const;
    Vector2 operator/(float val) const;
    ///@}

    /// Calculates the dot product between this and the other vector
    float dotProduct(const Vector2& other) const;

    /// Normalizes the vector
    void normalize();
    /// Returns a copy of the vector, normalized
    Vector2 getNormalized() const;

    /// Sets the length of the vector
    void setLength(float len);
    /// Gets the length of the vector
    float getLength() const;
    /// Gets the squared length of the vector
    float getLengthSquared() const;

    /// Gets the distance between this vector and the other, as points in 2D space
    float getDistance(const Vector2& other) const;
    /// Gets the squared distance between this vector and the other, as points in 2D space
    float getDistanceSquared(const Vector2& other) const;

    /// Sets the angle of the vector
    void setAngle(float ang);
    /// Gets the angle of the vector
    float getAngle() const;
    /// Gets the angle between this vector and the other, as points in 2D space
    float getAngleTo(const Vector2& other) const;

    /// Rotates this vector with the specified number of radians
    void rotate(float radAng);
    /// Gets a rotated copy of the vector
    Vector2 getRotated(float ang) const;
    /// Gets a perpendicular copy of the vector
    Vector2 getPerpendicular() const;

    float X, ///< The X value of the vector
          Y; ///< The Y value of the vector
};

}

#include "Vector.inl"
