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

class InputManager
{
public:
    InputManager(uint8_t count = 0);
    ~InputManager();

    void setBindCount(uint8_t count);

    void bindInput(uint8_t id);
    void bindInput(uint8_t id, const Input::Bind& bind);
    bool isBinding() const;

    template<typename T>
    void setCurve(sf::Joystick::Axis axis);

    void setDeadzone(sf::Joystick::Axis axis, float zone);
    float getDeadzone(sf::Joystick::Axis axis) const;

    Input& operator[](uint8_t id);
    const Input& at(uint8_t id) const;

    void handleEvent(const sf::Event& ev);
    void update();

    void disable(bool disable = true);
    bool isDisabled() const;

private:
    bool mDisabled;
    Input* mCurrentlyBinding;
    std::vector<Input> mInputs;

    std::vector<JoystickCurve*> mCurvesPerAxis;
};

}

#include "InputManager.inl"
