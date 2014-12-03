#pragma once

namespace Math
{

/** \brief A helper class for storing an euler angle as a complex number
 *
 * When storking an angle as a complex number, then you don't have to worry
 * about gimbal lock issues when you pass over wraparound points.
 */
struct Spinor
{
    /// Zeroing constructor
    Spinor();
    /// Construct based on an angle
    Spinor(float ang);
    /// Construct from a real and an imaginary part
    Spinor(float real, float imaginary);
    /// Default copy constructor
    Spinor(const Spinor&) = default;
    /// Default destructor
    ~Spinor() = default;

    /// Comparison operator
    bool operator==(const Spinor& rhs);

    /// Default assignment operator
    Spinor& operator=(const Spinor& other) = default;

    ///\name Arithmetic assignment operators
    ///@{

    Spinor& operator+=(const Spinor& rhs);
    Spinor& operator-=(const Spinor& rhs);
    Spinor& operator*=(const Spinor& rhs);
    Spinor& operator*=(float val);
    Spinor& operator/=(const Spinor& rhs);
    Spinor& operator/=(float val);

    ///@}

    ///\name Arithmetic operators
    ///@{

    Spinor operator+(const Spinor& rhs) const;
    Spinor operator-(const Spinor& rhs) const;
    Spinor operator*(const Spinor& rhs) const;
    Spinor operator*(float val) const;
    Spinor operator/(const Spinor& rhs) const;
    Spinor operator/(float val) const;

    ///@}

    /// Invert the Spinor
    void invert();
    /// Get an inverted copy of the Spinor
    Spinor getInverted() const;
    /// Normalize the length of the Spinor
    void normalize();
    /// Get a normalized copy of the Spinor
    Spinor getNormalized() const;
    
    /// Set the length of the Spinor
    void setLength(float len);
    /// Get the length of the Spinor
    float getLength() const;
    /// Set the squared length of the Spinor
    void setLengthSquared(float len);
    /// Get the squared length of the Spinor
    float getLengthSquared() const;

    /// Set the angle of the Spinor
    void setAngle(float ang);
    /// Get the angle of the Spinor
    float getAngle() const;

    /// Get the dot product of the Spinor
    float dotProduct(const Spinor& other) const;

    /** \brief Interpolate between this and another Spinor
     * 
     * This function will choose either linear or spherical interpolation based on the values given to it.
     */
    Spinor slerp(const Spinor& other, float delta) const;

    /// Real part of the value
    float Real, Complex;
    ///< Complex/Imaginary part of the value
};

}
