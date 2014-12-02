#include "InputManager.hpp"
#include <SFML/Window/Event.hpp>
#include <unordered_map>

using namespace Input;

Manager InputManager;

namespace
{
    std::unordered_map<sf::Keyboard::Key, sf::Keyboard::Key> OppositeKeys = {
        { sf::Keyboard::W, sf::Keyboard::S },
        { sf::Keyboard::S, sf::Keyboard::W },
        { sf::Keyboard::A, sf::Keyboard::D },
        { sf::Keyboard::D, sf::Keyboard::A },
        { sf::Keyboard::Up, sf::Keyboard::Down },
        { sf::Keyboard::Down, sf::Keyboard::Up },
        { sf::Keyboard::Left, sf::Keyboard::Right },
        { sf::Keyboard::Right, sf::Keyboard::Left }
    };
}

Manager::Manager() :
    mDisabled(false), mBindLinked(false), mCurrentlyBinding(nullptr)
{
    mCurvesPerAxis.resize(sf::Joystick::AxisCount);
    for (uint32_t i = 0; i < sf::Joystick::AxisCount; ++i)
        mCurvesPerAxis[i] = new Curves::Linear();
}

Manager::~Manager()
{
    for (auto& ptr : mCurvesPerAxis)
        delete ptr;
}

void Manager::setBindCount(uint8_t count)
{
    mInputs.resize(count);
}

void Manager::bindInput(uint8_t id, bool alsoLinked)
{
    mCurrentlyBinding = &mInputs.at(id);
    mBindLinked = alsoLinked;
}

void Manager::bindInput(uint8_t id, const ::Input::Input::Bind& bind, bool alsoLinked)
{
    auto& input = mInputs.at(id);

    input.mBind = bind;
    input.mValue = 0;

    if (alsoLinked && input.mLinkedInput)
    {
        auto* linked = input.mLinkedInput;
        switch (bind.Type)
        {
        case Input::Bind::Bind_Keyboard:
            if (OppositeKeys.count(bind.Data.Keyboard.Key) > 0)
            {
                linked->mBind.Type = Input::Bind::Bind_Keyboard;
                linked->mBind.Data.Keyboard.Key = OppositeKeys[bind.Data.Keyboard.Key];
                linked->mBind.Data.Keyboard.Modifiers = bind.Data.Keyboard.Modifiers;
            }
            break;

        case Input::Bind::Bind_Joystick_Axis:
            linked->mBind.Type = Input::Bind::Bind_Joystick_Axis;
            linked->mBind.Data.JoystickAxis = bind.Data.JoystickAxis;
            linked->mBind.Data.JoystickAxis.Positive = !bind.Data.JoystickAxis.Positive;
            break;
        }
    }
}

bool Manager::isBinding() const
{
    return mCurrentlyBinding != nullptr;
}

void Manager::linkInputs(uint8_t inputA, uint8_t inputB)
{
    mInputs[inputA].mLinkedInput = &mInputs[inputB];
    mInputs[inputB].mLinkedInput = &mInputs[inputA];
}

void Manager::setDeadzone(sf::Joystick::Axis axis, float zone)
{
    if (zone < 0 || zone >= 1)
        return;

    mCurvesPerAxis[axis]->Deadzone = zone;
}
float Manager::getDeadzone(sf::Joystick::Axis axis) const
{
    return mCurvesPerAxis[axis]->Deadzone;
}

::Input::Input& Manager::operator[](uint8_t id)
{
    return mInputs.at(id);
}
const ::Input::Input& Manager::operator[](uint8_t id) const
{
    return mInputs.at(id);
}
::Input::Input& Manager::at(uint8_t id)
{
    return mInputs.at(id);
}
const ::Input::Input& Manager::at(uint8_t id) const
{
    return mInputs.at(id);
}

void Manager::handleEvent(const sf::Event& ev)
{
    if (mDisabled || !mCurrentlyBinding)
        return;

    switch (ev.type)
    {
    case sf::Event::KeyPressed:
    {
        // Don't allow binding any of the modifier keys, system, or menu
        if (ev.key.code >= sf::Keyboard::LControl && ev.key.code <= sf::Keyboard::Menu)
            break;

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

        if (mBindLinked && mCurrentlyBinding->mLinkedInput && OppositeKeys.count(ev.key.code) > 0)
        {
            auto* linked = mCurrentlyBinding->mLinkedInput;

            linked->mBind.Type = Input::Bind::Bind_Keyboard;
            linked->mBind.Data.Keyboard.Key = OppositeKeys[ev.key.code];
            linked->mBind.Data.Keyboard.Modifiers = currentKeyModifiers;
        }

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

            if (mBindLinked && mCurrentlyBinding->mLinkedInput)
            {
                auto* linked = mCurrentlyBinding->mLinkedInput;

                linked->mBind.Type = Input::Bind::Bind_Joystick_Axis;
                linked->mBind.Data.JoystickAxis.ID = ev.joystickMove.joystickId;
                linked->mBind.Data.JoystickAxis.Axis = ev.joystickMove.axis;
                linked->mBind.Data.JoystickAxis.Positive = pos < 0;
            }

            mCurrentlyBinding = nullptr;
        }
    } break;
    }
}
void Manager::update()
{
    if (mDisabled || mCurrentlyBinding != nullptr)
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

void Manager::disable(bool disable)
{
    mDisabled = disable;

    if (mDisabled)
    {
        for (auto& input : mInputs)
            input.mValue = 0;
    }
}
bool Manager::isDisabled() const
{
    return mDisabled;
}