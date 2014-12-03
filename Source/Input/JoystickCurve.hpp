#pragma once

#include <cstdint>

namespace Input
{

/// A storage class for a joystick input curve, deadzone, and sensitivity values
struct JoystickCurve
{
    /// Default, zeroing, constructor
    JoystickCurve();
    /// No copying
    JoystickCurve(const JoystickCurve&) = delete;
    /// No assigning
    JoystickCurve& operator=(const JoystickCurve&) = delete;

    /// Apply the curve + deadzone + sensitivity to the input value
    virtual float apply(float input) const = 0;

    /// The deadzone percentage [0..1]
    float Deadzone;
    /// The sensitivity percentage [0..255]
    uint8_t Sensitivity;
};

/// A series of default curves
namespace Curves
{
    ///\brief This curve is always linear, regardless of sensitivity
    ///
    ///\note f(I) = I
    struct Linear : public JoystickCurve
    {
        float apply(float i) const;
    };

    ///\brief An okay default curve
    ///
    ///\note f(I) = I ^ (3 - (S / 128) )
    struct Default : public JoystickCurve
    {
        float apply(float i) const;
    };

    ///\brief An exponential curve
    ///
    ///\note f(I) = (exp((256 - s) * I) - 1) / (exp(256 - s) - 1)
    struct Exponential : public JoystickCurve
    {
        float apply(float i) const;
    };

    ///\brief A mixed curve
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
