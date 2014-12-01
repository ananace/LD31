#pragma once

namespace Math
{
    const float PI = 3.14159f;
    const float HALF_PI = PI / 2.f;
    const float DOUBLE_PI = PI * 2.f;
    const float PHI = 0.61803f;

    const float Deg2Rad = PI / 180.f;
    const float Rad2Deg = 180.f / PI;

    template<int MaxUlpError>
    bool FloatULPCompare(const float A, const float B);

    bool FloatCompare(const float a, const float b, const float EPSILON);

    template<typename T>
    T Lerp(const T& start, const T& end, float delta);
    template<typename T>
    T Slerp(const T& start, const T& end, float delta);

    float SlerpAngle(float start, float end, float delta);
}

#include "Common.inl"
