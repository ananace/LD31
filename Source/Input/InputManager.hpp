#pragma once

#include "Input.hpp"
#include "JoystickCurve.hpp"

#include <vector>

namespace sf
{
    class Event;
}

namespace Input
{

static class Manager
{
public:
    Manager();
    ~Manager();

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

    /** \brief Link two inputs together
     *
     * This will cause a change of bind for inputA to affect inputB
     */
    void linkInputs(uint8_t inputA, uint8_t inputB);

    template<typename T>
    void setCurve(sf::Joystick::Axis axis);

    void setDeadzone(sf::Joystick::Axis axis, float zone);
    float getDeadzone(sf::Joystick::Axis axis) const;

    const Input& operator[](uint8_t id) const;
    const Input& at(uint8_t id) const;

    void handleEvent(const sf::Event& ev);
    void update();

    void disable(bool disable = true);
    bool isDisabled() const;

private:
    bool mDisabled, mBindLinked;
    Input* mCurrentlyBinding;
    std::vector<Input> mInputs;

    std::vector<JoystickCurve*> mCurvesPerAxis;
} InputManager;

}

#include "InputManager.inl"
