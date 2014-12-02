#pragma once

#include <cstdint>

namespace Input
{

struct JoystickCurve
{
    JoystickCurve();

    virtual float apply(float input) const = 0;

    float Deadzone;
    uint8_t Sensitivity;
};

namespace Curves
{
    /// This curve does nothing to the input
    ///
    ///\note f(I) = I
    struct Linear : public JoystickCurve
    {
        float apply(float i) const;
    };

    /// An okay default curve
    ///
    ///\note f(I) = I ^ (3 - (S / 128) )
    struct Default : public JoystickCurve
    {
        float apply(float i) const;
    };

    /// An exponential curve
    ///
    ///\note f(I) = (exp((256 - s) * I) - 1) / (exp(256 - s) - 1)
    struct Exponential : public JoystickCurve
    {
        float apply(float i) const;
    };

    /// A mixed curve
    ///
    /// It will act like a positive power curve at s < 128,
    /// become linear at s == 128, and then take a fractional shape at s > 128
    ///\note f(I) = I ^ (1 + ((128 - s) / 256))
    struct Mixed : public JoystickCurve
    {
        float apply(float i) const;
    };
}

}
