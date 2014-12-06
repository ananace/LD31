#pragma once

#include "Input.hpp"
#include "JoystickCurve.hpp"
#include <Math/Vector.hpp>

#include <vector>

namespace sf
{
    class Event;
}

namespace Input
{

/** \brief A class for managing inputs and binds
 *
 */
extern class InputMan
{
public:
    /// Constructor
    InputMan();
    /// Not copyable
    InputMan(const InputMan&) = delete;
    /// Destructor
    ~InputMan();

    /// Set the number of binds for the manager
    void setBindCount(uint8_t count);

    /// Starts binding the selected input, next input will bind it
    void bindInput(uint8_t id, bool alsoLinked = true);
    /// Binds the selected input with the specified bind data
    void bindInput(uint8_t id, const Input::Bind& bind, bool alsoLinked = true);
    /// Get the currently binding input or nullptr if none
    const Input* getBinding() const;
    /// Check if the manager is currently binding an input
    bool isBinding() const;
    bool isBindingLinked() const;

    /** \brief Link two inputs together
     *
     * This will cause a change of binding for inputA to affect inputB
     */
    void linkInputs(uint8_t inputA, uint8_t inputB);

    /// Sets the joystick curve for the specified axis
    template<typename T>
    void setCurve(sf::Joystick::Axis axis);

    ///\brief Sets a deadzone for the specified axis
    ///\note This value is globally applied, for all joysticks
    void setDeadzone(sf::Joystick::Axis axis, float zone);
    /// Gets the deadzone value for the specified axis
    float getDeadzone(sf::Joystick::Axis axis) const;
    /// Sets a sensitivity value for the specified axis
    ///\note This value is globally applied, for all joysticks
    void setSensitivity(sf::Joystick::Axis axis, uint8_t sensitivity);
    /// Gets the sensitivity value for the given axis
    uint8_t getSensitivity(sf::Joystick::Axis axis) const;

    /// Get the mouse position relative to the main window
    Math::Vector2 getMousePos() const;
    int getMouseWheelDelta() const;
    int getMouseWheelPos() const;

    /// Gets the Input at the specified ID
    const Input& operator[](uint8_t id) const;
    /// Gets the Input at the specified ID
    const Input& at(uint8_t id) const;

    /// Handle an event, only used for binding
    void handleEvent(const sf::Event& ev);
    ///\brief Updates all the input values
    ///\note For best results, run this once in the start of the update loop
    void update();

    ///\brief Disable or enable the input system
    ///\note When disabling the input system, all input values will be zeroed for the duration
    void disable(bool disable = true);
    /// Check if the input system is disabled
    bool isDisabled() const;

private:
    bool mDisabled, ///< Is the input system disabled
         mBindLinked; ///< Should the next bind also applied to the linked Input

    Math::Vector2 mMousePos; ///< The current mouse position
    int mMouseWheelDelta,
        mMouseWheelPos;

    Input* mCurrentlyBinding; ///< The Input currently being bound (or nullptr if none)
    std::vector<Input> mInputs; ///< The list of available inputs

    std::vector<JoystickCurve*> mCurvesPerAxis; ///< The joystic curves, stored by axis
} InputManager; ///< The default global instance

}

#include "InputManager.inl"
