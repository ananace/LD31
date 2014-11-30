#include "Common.hpp"

#include <cassert>
#include <cmath>

using namespace Math;

bool Math::FloatCompare(const float a, const float b, const float EPSILON)
{
    assert(EPSILON > 0 && "Epsilon value must be positive");

    return abs(a - b) < EPSILON;
}