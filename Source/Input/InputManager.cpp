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
    std::unordered_map<sf::Keyboard::Key, sf::Keyboard::Key, std::hash<int>> OppositeKeys = {
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
    const Input::Input* getLinked(const Input::Input* in)
    {
        return &in->getLinked();
    }

    bool isBindKeyboard(const Input::Input::Bind& in)
    {
        return in.Type == Input::Input::Bind::Bind_Keyboard;    
    }
    sf::Keyboard::Key getBindKeyboard(const Input::Input::Bind& in)
    {
        return in.Keyboard.Key;
    }
    bool isBindJoyAxis(const Input::Input::Bind& in)
    {
        return in.Type == Input::Input::Bind::Bind_Joystick_Axis;
    }
    sf::Joystick::Axis getBindJoystickAxis(const Input::Input::Bind& in)
    {
        return in.JoystickAxis.Axis;
    }
    bool getBindJoystickAxisPositive(const Input::Input::Bind& in)
    {
        return in.JoystickAxis.Positive;
    }
    bool isBindJoyButton(const Input::Input::Bind& in)
    {
        return in.Type == Input::Input::Bind::Bind_Joystick_Button;
    }

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;

            r = eng->SetDefaultNamespace("Input"); assert(r >= 0);

            r = eng->RegisterObjectType("Bind", sizeof(Input::Input::Bind), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Input::Input::Bind>()); assert(r >= 0);

            r = eng->RegisterObjectMethod("Bind", "bool get_Keyboard() const", asFUNCTION(isBindKeyboard), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Bind", "bool get_JoystickAxis() const", asFUNCTION(isBindKeyboard), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Bind", "bool get_JoystickButton() const", asFUNCTION(isBindKeyboard), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->RegisterObjectMethod("Bind", "Keyboard::Key KeyboardKey() const", asFUNCTION(getBindKeyboard), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Bind", "Joystick::Axis JoystickAxis() const", asFUNCTION(getBindJoystickAxis), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Bind", "bool JoystickAxisPositive() const", asFUNCTION(getBindKeyboard), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Bind", "uint JoystickButton() const", asFUNCTION(getBindKeyboard), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->SetDefaultNamespace(""); assert(r >= 0);

            r = eng->RegisterObjectType("Input", 0, asOBJ_REF | asOBJ_NOCOUNT); assert(r >= 0);

            r = eng->RegisterObjectMethod("Input", "bool opImplConv()", asMETHOD(Input::Input, operator bool), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Input", "float opConv()", asMETHOD(Input::Input, operator float), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Input", "float get_Value()", asMETHOD(Input::Input, getValue), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Input", "float get_CombinedValue()", asMETHOD(Input::Input, getCombinedValue), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Input", "bool get_Pressed()", asMETHOD(Input::Input, isPressed), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Input", "bool get_Linked()", asMETHOD(Input::Input, isLinked), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Input", "Input@ GetLinked()", asFUNCTION(getLinked), asCALL_CDECL_OBJLAST); assert(r >= 0);


            r = eng->SetDefaultNamespace("Inputs"); assert(r >= 0);
            
            r = eng->RegisterGlobalFunction("bool get_Disabled()", asMETHOD(Input::InputMan, isDisabled), asCALL_THISCALL_ASGLOBAL, &Input::InputManager); assert(r >= 0);
            r = eng->RegisterGlobalFunction("void set_Disabled(bool)", asMETHOD(Input::InputMan, disable), asCALL_THISCALL_ASGLOBAL, &Input::InputManager); assert(r >= 0);

            r = eng->RegisterGlobalFunction("Input@ GetBinding()", asFUNCTION(getBinding), asCALL_CDECL); assert(r >= 0);
            r = eng->RegisterGlobalFunction("bool get_Binding()", asMETHOD(Input::InputMan, isBinding), asCALL_THISCALL_ASGLOBAL, &Input::InputManager); assert(r >= 0);
            r = eng->RegisterGlobalFunction("bool get_BindingLinked()", asMETHOD(Input::InputMan, isBindingLinked), asCALL_THISCALL_ASGLOBAL, &Input::InputManager); assert(r >= 0);

            r = eng->RegisterGlobalFunction("void StartBind(uint8,bool = true)", asMETHODPR(Input::InputMan, bindInput, (uint8_t, bool), void), asCALL_THISCALL_ASGLOBAL, &Input::InputManager); assert(r >= 0);

            r = eng->RegisterGlobalFunction("Input@ GetInput(uint8)", asFUNCTION(getInput), asCALL_CDECL); assert(r >= 0);

            r = eng->SetDefaultNamespace(""); assert(r >= 0);
        }, 6);
        return true;
    }
}

bool Script::ScriptExtensions::InputExtensions = Reg();

InputMan::InputMan() :
    mDisabled(false), mBindLinked(false), mMouseWheelDelta(0), mMouseWheelPos(0), mCurrentlyBinding(nullptr)
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
            if (OppositeKeys.count(bind.Keyboard.Key) > 0)
            {
                linked->mBind.Type = Input::Bind::Bind_Keyboard;
                linked->mBind.Keyboard.Key = OppositeKeys[bind.Keyboard.Key];
                linked->mBind.Keyboard.Modifiers = bind.Keyboard.Modifiers;
            }
            break;

        case Input::Bind::Bind_Joystick_Axis:
            linked->mBind.Type = Input::Bind::Bind_Joystick_Axis;
            linked->mBind.JoystickAxis = bind.JoystickAxis;
            linked->mBind.JoystickAxis.Positive = !bind.JoystickAxis.Positive;
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

bool InputMan::isBindingLinked() const
{
    return mCurrentlyBinding != nullptr && mBindLinked;
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
void InputMan::setSensitivity(sf::Joystick::Axis axis, uint8_t s)
{
    mCurvesPerAxis[axis]->Sensitivity = s;
}
uint8_t InputMan::getSensitivity(sf::Joystick::Axis axis) const
{
    return mCurvesPerAxis[axis]->Sensitivity;
}

Math::Vector2 InputMan::getMousePos() const
{
    return mMousePos;
}
int InputMan::getMouseWheelDelta() const
{
    return mMouseWheelDelta;
}
int InputMan::getMouseWheelPos() const
{
    return mMouseWheelPos;
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
    if (ev.type == sf::Event::MouseMoved)
    {
        mMousePos.X = (float)ev.mouseMove.x;
        mMousePos.Y = (float)ev.mouseMove.y;
        return;
    }
    else if (ev.type == sf::Event::MouseWheelMoved)
    {
        mMouseWheelDelta = ev.mouseWheel.delta;
        mMouseWheelPos += mMouseWheelDelta;
    }

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
        mCurrentlyBinding->mBind.Keyboard.Key = ev.key.code;
        mCurrentlyBinding->mBind.Keyboard.Modifiers = currentKeyModifiers;

        if (mBindLinked && mCurrentlyBinding->mLinkedInput && OppositeKeys.count(ev.key.code) > 0)
        {
            auto* linked = mCurrentlyBinding->mLinkedInput;

            linked->mBind.Type = Input::Bind::Bind_Keyboard;
            linked->mBind.Keyboard.Key = OppositeKeys[ev.key.code];
            linked->mBind.Keyboard.Modifiers = currentKeyModifiers;
        }

        mCurrentlyBinding = nullptr;
    } break;

    case sf::Event::JoystickButtonPressed:
    {
        mCurrentlyBinding->mBind.Type = Input::Bind::Bind_Joystick_Button;
        mCurrentlyBinding->mBind.JoystickButton.ID = ev.joystickButton.joystickId;
        mCurrentlyBinding->mBind.JoystickButton.Button = ev.joystickButton.button;

        mCurrentlyBinding = nullptr;
    } break;

    case sf::Event::JoystickMoved:
    {
        float pos = ev.joystickMove.position;
        if (abs(pos) > Input::PRESS_PERCENTAGE * 100.f)
        {
            mCurrentlyBinding->mBind.Type = Input::Bind::Bind_Joystick_Axis;
            mCurrentlyBinding->mBind.JoystickAxis.ID = ev.joystickMove.joystickId;
            mCurrentlyBinding->mBind.JoystickAxis.Axis = ev.joystickMove.axis;
            mCurrentlyBinding->mBind.JoystickAxis.Positive = pos > 0;

            if (mBindLinked && mCurrentlyBinding->mLinkedInput)
            {
                auto* linked = mCurrentlyBinding->mLinkedInput;

                linked->mBind.Type = Input::Bind::Bind_Joystick_Axis;
                linked->mBind.JoystickAxis.ID = ev.joystickMove.joystickId;
                linked->mBind.JoystickAxis.Axis = ev.joystickMove.axis;
                linked->mBind.JoystickAxis.Positive = pos < 0;
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
        auto& data = input.mBind;

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