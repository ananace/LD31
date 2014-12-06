#pragma once

namespace Math
{
    /// The Pi constant (~3.1415)
    const float PI = 3.14159f;
    /// Half a Pi
    const float HALF_PI = PI / 2.f;
    /// Twice a Pi
    const float DOUBLE_PI = PI * 2.f;
    /// The golden ratio, Phi
    const float PHI = 0.61803f;

    /// The value for converting degrees to radians
    const float Deg2Rad = PI / 180.f;
    /// The value for converting radians to degrees
    const float Rad2Deg = 180.f / PI;

    /// Floating point comparison using units-in-last-place
    template<int MaxUlpError>
    bool FloatULPCompare(const float A, const float B);

    /// Floating point comparison using an epsilon value
    bool FloatCompare(const float a, const float b, const float EPSILON);

    float Random(float a, float b);
    int Random(int a, int b);

    ///\brief Linear interpolation
    ///\note Same as `start * (1 - delta) + end * delta` 
    template<typename T>
    T Lerp(const T& start, const T& end, float delta);
    ///\brief Spherical/Linear interpolation
    ///\note Only works on classes that define a `slerp()` function
    ///\sa Spinor::slerp()
    template<typename T>
    T Slerp(const T& start, const T& end, float delta);

    ///\brief Interpolates between two angles using spherical or linear interpolation
    ///\note This uses the \a Spinor class internally, to solve euler angle issues
    float SlerpAngle(float start, float end, float delta);
}

#include "Common.inl"
