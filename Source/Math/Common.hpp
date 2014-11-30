#pragma once

namespace Math
{
    template<int MaxUlpError>
    bool FloatULPCompare(const float A, const float B);

    bool FloatCompare(const float a, const float b, const float EPSILON);
}

#include "Common.inl"
