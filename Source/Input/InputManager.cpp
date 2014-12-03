#include "InputManager.hpp"
#include <Script/ScriptExtensions.hpp>

#include <SFML/Window/Event.hpp>

#include <angelscript.h>

#include <unordered_map>
#include <cassert>

using Input::InputMan;

InputMan Input::InputManager;

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

    const Input::Input* getBinding()
    {
        return Input::InputManager.getBinding();
    }
    const Input::Input* getInput(uint32_t inp)
    {
        return &Input::InputManager[inp];
    }
    void bindInput(uint8_t inp, bool autoLink)
    {
        Input::InputManager.bindInput(inp, autoLink);
    }

    const Input::Input* getLinked(const Input::Input* in)
    {
        return &in->getLinked();
    }

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;

            r = eng->RegisterObjectType("Input", 0, asOBJ_REF | asOBJ_NOCOUNT); assert(r >= 0);

            r = eng->RegisterObjectMethod("Input", "bool opImplConv()", asMETHOD(Input::Input, operator bool), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Input", "float opConv()", asMETHOD(Input::Input, operator float), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Input", "float get_Value()", asMETHOD(Input::Input, getValue), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Input", "bool get_Pressed()", asMETHOD(Input::Input, isPressed), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Input", "bool get_Linked()", asMETHOD(Input::Input, isLinked), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Input", "Input@ GetLinked()", asFUNCTION(getLinked), asCALL_CDECL_OBJLAST); assert(r >= 0);


            r = eng->SetDefaultNamespace("Inputs"); assert(r >= 0);
            
            r = eng->RegisterGlobalFunction("Input@ GetBinding()", asFUNCTION(getBinding), asCALL_CDECL); assert(r >= 0);
            r = eng->RegisterGlobalFunction("bool IsBinding()", asFUNCTION(getInput), asCALL_CDECL); assert(r >= 0);
            r = eng->RegisterGlobalFunction("void StartBind(uint8,bool = true)", asFUNCTION(bindInput), asCALL_CDECL); assert(r >= 0);

            r = eng->RegisterGlobalFunction("Input@ GetInput(uint8)", asFUNCTION(getInput), asCALL_CDECL); assert(r >= 0);

            r = eng->SetDefaultNamespace(""); assert(r >= 0);
        });
        return true;
    }
}

bool Script::ScriptExtensions::InputExtensions = Reg();

InputMan::InputMan() :
    mDisabled(false), mBindLinked(false), mCurrentlyBinding(nullptr)
{
    mCurvesPerAxis.resize(sf::Joystick::AxisCount);
    for (uint32_t i = 0; i < sf::Joystick::AxisCount; ++i)
        mCurvesPerAxis[i] = new Curves::Linear();
}

InputMan::~InputMan()
{
    for (auto& ptr : mCurvesPerAxis)
        delete ptr;
}

void InputMan::setBindCount(uint8_t count)
{
    mInputs.resize(count);
}

void InputMan::bindInput(uint8_t id, bool alsoLinked)
{
    mCurrentlyBinding = &mInputs.at(id);
    mBindLinked = alsoLinked;
}

void InputMan::bindInput(uint8_t id, const Input::Input::Bind& bind, bool alsoLinked)
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

const Input::Input* InputMan::getBinding() const
{
    return mCurrentlyBinding;
}

bool InputMan::isBinding() const
{
    return mCurrentlyBinding != nullptr;
}

void InputMan::linkInputs(uint8_t inputA, uint8_t inputB)
{
    mInputs[inputA].mLinkedInput = &mInputs[inputB];
    mInputs[inputB].mLinkedInput = &mInputs[inputA];
}

void InputMan::setDeadzone(sf::Joystick::Axis axis, float zone)
{
    if (zone < 0 || zone >= 1)
        return;

    mCurvesPerAxis[axis]->Deadzone = zone;
}
float InputMan::getDeadzone(sf::Joystick::Axis axis) const
{
    return mCurvesPerAxis[axis]->Deadzone;
}

const Input::Input& InputMan::operator[](uint8_t id) const
{
    return mInputs.at(id);
}
const Input::Input& InputMan::at(uint8_t id) const
{
    return mInputs.at(id);
}

void InputMan::handleEvent(const sf::Event& ev)
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
void InputMan::update()
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

void InputMan::disable(bool disable)
{
    mDisabled = disable;

    if (mDisabled)
    {
        for (auto& input : mInputs)
            input.mValue = 0;
    }
}
bool InputMan::isDisabled() const
{
    return mDisabled;
}