#pragma once

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

    int intDiff = abs(aInt - bInt);

    if (intDiff <= MaxUlpError)
        return true;

    return false;
}

}