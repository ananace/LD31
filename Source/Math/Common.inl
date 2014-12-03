#pragma once

#include <cmath>

namespace Math
{

template<int MaxUlpError>
inline bool FloatULPCompare(const float A, const float B)
{
    static_assert(MaxUlpError > 0 && MaxUlpError < 4 * 1024 * 1024, "Max 'Units in Last Place' error must be a valid number");

    int aInt = *(int*)&A;

    if (aInt < 0)
        aInt = 0x80000000 - aInt;

    int bInt = *(int*)&B;

    if (bInt < 0)
        bInt = 0x80000000 - bInt;

    int intDiff = std::abs(aInt - bInt);

    if (intDiff <= MaxUlpError)
        return true;

    return false;
}

template<typename T>
inline T Lerp(const T& start, const T& end, float delta)
{
    return (T)(start * (1 - delta) + end * delta);
}

template<typename T>
inline T Slerp(const T& start, const T& end, float delta)
{
    return start.slerp(end, delta);
}

}