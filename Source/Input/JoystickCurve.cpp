#include "JoystickCurve.hpp"

#include <cmath>

using namespace Input::Curves;

Input::JoystickCurve::JoystickCurve() :
    Deadzone(0), Sensitivity(0)
{
}

float Linear::apply(float i) const
{
    if (i < Deadzone)
        return 0;

    return i;
}
float Default::apply(float i) const
{
    if (i < Deadzone)
        return 0;

    return pow(i, (3 - (Sensitivity / 128)));
}
float Exponential::apply(float i) const
{
    if (i < Deadzone)
        return 0;

    return (exp((256 - Sensitivity) * i) - 1) / (exp(256.f - Sensitivity) - 1);
}
float Mixed::apply(float i) const
{
    if (i < Deadzone)
        return 0;

    return pow(i, (1 + ((128 - Sensitivity) / 256)));
}
