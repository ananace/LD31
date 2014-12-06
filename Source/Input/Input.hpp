#pragma once

#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstdint>

namespace Input
{

class InputMan;

/** \brief Storage class for a bound input
 *
 * \note This class is just a series of values, and such can be copied
 */
class Input
{
public:
    ///\brief Container for the bind data
    struct Bind
    {
        /// Which modifiers are applied to the key
        enum KeyboardModifiers : uint8_t
        {
            Modifier_None = 0, ///< No modifiers

            Modifier_LShift = 1 << 0, ///< Left shift must be pressed
            Modifier_RShift = 1 << 1, ///< Right shift must be pressed
            Modifier_Shift = Modifier_LShift | Modifier_RShift, ///< Either shift key must be pressed

            Modifier_LControl = 1 << 2, ///< Left control must be pressed
            Modifier_RControl = 1 << 3, ///< Right control must be pressed
            Modifier_Control = Modifier_LControl | Modifier_RControl, ///< Either control key must be pressed

            Modifier_LAlt = 1 << 4, ///< Left alt must be pressed
            Modifier_RAlt = 1 << 5, ///< Right alt must bbe pressed
            Modifier_Alt = Modifier_LAlt | Modifier_RAlt ///< Either alt must be pressed
        };

        /// The type of bind
        enum {
            Bind_None, ///< Unbound
            Bind_Keyboard, ///< Bound to a keyboard key + modifier
            Bind_Joystick_Axis, ///< Bound to an axis on a joystick
            Bind_Joystick_Button ///< Bound to a button on a joystick
        } Type;

        /// The bind data itself
        union
        {
            struct {
                sf::Keyboard::Key Key; ///< The keyboard key
                uint8_t Modifiers; ///< The required modifiers
                                   ///< \sa KeyboardModifiers
            } Keyboard; ///< A bind to a keyboard key
            struct {
                uint32_t ID; ///< The joystick ID
                sf::Joystick::Axis Axis; ///< The Axis
                bool Positive; ///< Is the bind for the positive half
            } JoystickAxis; ///< A bind to (half) a joystick axis
            struct {
                uint32_t ID, ///< The joystick ID
                         Button; ///< The Button of the joystick
            } JoystickButton; ///< A bind to a joystick button
        };
    };

    ///\brief What percentage should count as "pressed"
    static const float PRESS_PERCENTAGE;

    /// Zeroing contructor
    Input();
    /// Default copy constructor
    Input(const Input&) = default;
    /// Default destructor
    ~Input() = default;

    /// Default assignment operator
    Input& operator=(const Input&) = default;

    /** @name Quick-access functions
     * \note Inlined for speed
     */
    ///@{

    /// Deref the input to get its exact value. [0..1]
    inline float operator*() const { return mValue; }
    /// Cast to float to get its exact value. [0..1]
    inline explicit operator float() const { return mValue; }
    /// Implicitly cast to bool to check if it's pressed or not. (> #PRESS_PERCENTAGE )
    inline operator bool() const { return mValue >= PRESS_PERCENTAGE; }
    /// ! operator for checking if the input's not pressed.
    inline bool operator!() const { return mValue < PRESS_PERCENTAGE; }

    ///@}
    
    /// Gets a copy of the bound data
    Bind getBind() const;
    /// Gets the linked input
    ///\warning This will error if you use it on an unlinked input
    const Input& getLinked() const;
    /// Gets the current value of the input
    float getValue() const;

    /** \brief Get the combined value of this input and its linked opposite
     *
     * This is the same as `getLinked().getValue() - getValue()`,
     * and it's intended for use as such:
     *
     * ~~~~~~~~~~~~~
     * enum Inputs {
     *   Input_Up,
     *   Input_Down,
     *   Input_Left,
     *   Input_Right,
     *
     *   Input_UpDown = Input_Up,
     *   Input_LeftRight = Input_Left
     * };
     *
     * // Code...
     *
     * Input::Manager& manager = Input::InputManager;
     * manager.setBindCount(4);
     * manager.linkInputs(Input_Up, Input_Down);
     * manager.linkInputs(Input_Left, Input_Right);
     *
     * // Code...
     * 
     * // -1 if up is pressed, 1 if down is pressed
     * float vertical = manager.at(Input_UpDown).getCombinedValue();
     * ~~~~~~~~~~~~~
     *
     *\note If the input is not linked, then this function is identical to \a getValue()
     */
    float getCombinedValue() const;
    int getCombinedPress() const;

    /// Returns if the input has a linked counterpart
    bool isLinked() const;
    /// Gets if the input is pressed or not (> #PRESS_PERCENTAGE )
    bool isPressed() const;

private:
    Input* mLinkedInput; ///< The linked input, if any
    float mValue; ///< The current value
    Bind mBind; ///< The bind data for the input

    friend class InputMan;
};

}
