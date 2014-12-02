#pragma once

#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstdint>

namespace Input
{

class Manager;

class Input
{
public:
    ///\brief Container for the bind data
    struct Bind
    {
        enum KeyboardModifiers : uint8_t
        {
            Modifier_None = 0,

            Modifier_LShift = 1 << 0,
            Modifier_RShift = 1 << 1,
            Modifier_Shift = Modifier_LShift | Modifier_RShift,

            Modifier_LControl = 1 << 2,
            Modifier_RControl = 1 << 3,
            Modifier_Control = Modifier_LControl | Modifier_RControl,

            Modifier_LAlt = 1 << 4,
            Modifier_RAlt = 1 << 5,
            Modifier_Alt = Modifier_LAlt | Modifier_RAlt
        };
        enum {
            Bind_None,
            Bind_Keyboard,
            Bind_Joystick_Axis,
            Bind_Joystick_Button
        } Type;

        union
        {
            struct {
                sf::Keyboard::Key Key;
                uint8_t Modifiers;
            } Keyboard;
            struct {
                uint32_t ID;
                sf::Joystick::Axis Axis;
                bool Positive;
            } JoystickAxis;
            struct {
                uint32_t ID,
                         Button;
            } JoystickButton;
        } Data;
    };

    ///\brief What percentage should count as "pressed"
    static const float PRESS_PERCENTAGE;

    Input();
    ~Input();

    /** @name Quick-access functions
     * \note Inlined for speed
     */
    ///@{

    /// Deref the input to get its exact value. [0..1]
    inline float operator*() const { return mValue; }
    /// Convert to bool to check if it's pressed or not. (> PRESS_PERCENTAGE)
    inline operator bool() const { return mValue >= PRESS_PERCENTAGE; }
    /// ! operator for checking if the input's not pressed.
    inline bool operator!() const { return mValue < PRESS_PERCENTAGE; }

    ///@}
    
    Bind getBind() const;
    const Input& getLinked() const;
    float getValue() const;

    bool isLinked() const;
    bool isPressed() const;

private:
    Input* mLinkedInput;
    float mValue;
    Bind mBind;

    friend class Manager;
};

}
