#pragma once

namespace Math
{

struct Spinor
{
    Spinor();
    Spinor(float ang);
    Spinor(float real, float complex);
    Spinor(const Spinor&) = default;
    ~Spinor() = default;

    // Comparison operator
    bool operator==(const Spinor& rhs);

    // Assignment operator
    Spinor& operator=(Spinor other);

    // Arithmetic assignment operators

    Spinor& operator+=(const Spinor& rhs);
    Spinor& operator-=(const Spinor& rhs);
    Spinor& operator*=(const Spinor& rhs);
    Spinor& operator*=(float val);
    Spinor& operator/=(const Spinor& rhs);
    Spinor& operator/=(float val);

    // Arithmetic operators

    Spinor operator+(const Spinor& rhs) const;
    Spinor operator-(const Spinor& rhs) const;
    Spinor operator*(const Spinor& rhs) const;
    Spinor operator*(float val) const;
    Spinor operator/(const Spinor& rhs) const;
    Spinor operator/(float val) const;

    // Useful functions

    void invert();
    Spinor getInverted() const;
    void normalize();
    Spinor getNormalized() const;
    
    void setLength(float len);
    float getLength() const;
    void setLengthSquared(float len);
    float getLengthSquared() const;

    void setAngle(float ang);
    float getAngle() const;

    float dotProduct(const Spinor& other) const;

    Spinor slerp(const Spinor& other, float delta) const;

    // Components

    float Real, Complex;
};

}
