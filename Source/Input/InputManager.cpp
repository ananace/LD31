#include "InputManager.hpp"
#include <SFML/Window/Event.hpp>

using namespace Input;

InputManager::InputManager(uint8_t inputCount) :
    mDisabled(false), mCurrentlyBinding(nullptr)
{
    mInputs.resize(inputCount);
    mCurvesPerAxis.resize(sf::Joystick::AxisCount);
    for (uint32_t i = 0; i < sf::Joystick::AxisCount; ++i)
        mCurvesPerAxis[i] = new Curves::Linear();
}

InputManager::~InputManager()
{
    for (auto& ptr : mCurvesPerAxis)
        delete ptr;
}

void InputManager::setBindCount(uint8_t count)
{
    mInputs.resize(count);
}

void InputManager::bindInput(uint8_t id)
{
    mCurrentlyBinding = &mInputs.at(id);
}

void InputManager::bindInput(uint8_t id, const ::Input::Input::Bind& bind)
{
    auto& input = mInputs.at(id);

    input.mBind = bind;
    input.mValue = 0;
}

bool InputManager::isBinding() const
{
    return mCurrentlyBinding != nullptr;
}

void InputManager::setDeadzone(sf::Joystick::Axis axis, float zone)
{
    if (zone < 0 || zone >= 1)
        return;

    mCurvesPerAxis[axis]->Deadzone = zone;
}
float InputManager::getDeadzone(sf::Joystick::Axis axis) const
{
    return mCurvesPerAxis[axis]->Deadzone;
}

::Input::Input& InputManager::operator[](uint8_t id)
{
    return mInputs.at(id);
}
const ::Input::Input& InputManager::at(uint8_t id) const
{
    return mInputs.at(id);
}

void InputManager::handleEvent(const sf::Event& ev)
{
    if (mDisabled || !mCurrentlyBinding)
        return;

    switch (ev.type)
    {
    case sf::Event::KeyPressed:
    {
        uint8_t currentKeyModifiers =
            (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ? Input::Bind::Modifier_LShift : 0) |
            (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) ? Input::Bind::Modifier_RShift : 0) |
            (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ? Input::Bind::Modifier_LControl : 0) |
            (sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) ? Input::Bind::Modifier_RControl : 0) |
            (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) ? Input::Bind::Modifier_LAlt : 0) |
            (sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt) ? Input::Bind::Modifier_RAlt : 0);

        mCurrentlyBinding->mBind.Type = Input::Bind::Bind_Keyboard;
        mCurrentlyBinding->mBind.Data.Keyboard.Key = ev.key.code;
        mCurrentlyBinding->mBind.Data.Keyboard.Modifiers = currentKeyModifiers;

        mCurrentlyBinding = nullptr;
    } break;

    case sf::Event::JoystickButtonPressed:
    {
        mCurrentlyBinding->mBind.Type = Input::Bind::Bind_Joystick_Button;
        mCurrentlyBinding->mBind.Data.JoystickButton.ID = ev.joystickButton.joystickId;
        mCurrentlyBinding->mBind.Data.JoystickButton.Button = ev.joystickButton.button;

        mCurrentlyBinding = nullptr;
    } break;

    case sf::Event::JoystickMoved:
    {
        float pos = ev.joystickMove.position;
        if (abs(pos) > Input::PRESS_PERCENTAGE * 100.f)
        {
            mCurrentlyBinding->mBind.Type = Input::Bind::Bind_Joystick_Axis;
            mCurrentlyBinding->mBind.Data.JoystickAxis.ID = ev.joystickMove.joystickId;
            mCurrentlyBinding->mBind.Data.JoystickAxis.Axis = ev.joystickMove.axis;
            mCurrentlyBinding->mBind.Data.JoystickAxis.Positive = pos > 0;

            mCurrentlyBinding = nullptr;
        }
    } break;
    }
}
void InputManager::update()
{
    if (mDisabled)
        return;

    uint8_t currentKeyModifiers =
        (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ? Input::Bind::Modifier_LShift : 0) |
        (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) ? Input::Bind::Modifier_RShift : 0) |
        (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ? Input::Bind::Modifier_LControl : 0) |
        (sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) ? Input::Bind::Modifier_RControl : 0) |
        (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) ? Input::Bind::Modifier_LAlt : 0) |
        (sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt) ? Input::Bind::Modifier_RAlt : 0);

    for (auto& input : mInputs)
    {
        float val = 0;
        auto& data = input.mBind.Data;

        switch (input.mBind.Type)
        {
        case Input::Bind::Bind_Keyboard:
            if ((data.Keyboard.Modifiers & currentKeyModifiers) == data.Keyboard.Modifiers)
                val = sf::Keyboard::isKeyPressed(data.Keyboard.Key);
            break;

        case Input::Bind::Bind_Joystick_Axis:
        {
            float pos = sf::Joystick::getAxisPosition(data.JoystickAxis.ID, data.JoystickAxis.Axis);

            if (pos > 0 && data.JoystickAxis.Positive)
                val = mCurvesPerAxis[data.JoystickAxis.Axis]->apply(pos / 100.f);
            else if (pos < 0 && !data.JoystickAxis.Positive)
                val = mCurvesPerAxis[data.JoystickAxis.Axis]->apply(pos / -100.f);
        } break;

        case Input::Bind::Bind_Joystick_Button:
            val = sf::Joystick::isButtonPressed(data.JoystickButton.ID, data.JoystickButton.Button);
            break;
        }

        input.mValue = val;
    }
}

void InputManager::disable(bool disable)
{
    mDisabled = disable;
}
bool InputManager::isDisabled() const
{
    return mDisabled;
}