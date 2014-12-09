#pragma once

#include "Extensions.hpp"
#include <Input/InputManager.hpp>
#include <Script/ScriptExtensions.hpp>

#include <SFML/Window/Mouse.hpp>

#include <angelscript.h>

#include <cassert>

namespace
{
#ifndef AS_SUPPORT_VALRET
    void getMousePos(asIScriptGeneric* gen)
    {
        new(gen->GetAddressOfReturnLocation()) Math::Vector2(Input::InputManager.getMousePos());
    }
#endif

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng){
            int r = 0;

            r = eng->SetDefaultNamespace("Mouse"); assert(r >= 0);

            r = eng->RegisterEnum("Button"); assert(r >= 0);
            r = eng->RegisterEnumValue("Button", "Left", sf::Mouse::Left); assert(r >= 0);
            r = eng->RegisterEnumValue("Button", "Middle", sf::Mouse::Middle); assert(r >= 0);
            r = eng->RegisterEnumValue("Button", "Right", sf::Mouse::Right); assert(r >= 0);
            r = eng->RegisterEnumValue("Button", "XButton1", sf::Mouse::XButton1); assert(r >= 0);
            r = eng->RegisterEnumValue("Button", "XButton2", sf::Mouse::XButton2); assert(r >= 0);

#ifdef AS_SUPPORT_VALRET
            r = eng->RegisterGlobalFunction("Vec2 get_Position()", asMETHOD(Input::InputMan, getMousePos), asCALL_THISCALL_ASGLOBAL, &Input::InputManager); assert(r >= 0);
#else
            r = eng->RegisterGlobalFunction("Vec2 get_Position()", asFUNCTION(getMousePos), asCALL_GENERIC); assert(r >= 0);
#endif
            r = eng->RegisterGlobalFunction("int get_WheelDelta()", asMETHOD(Input::InputMan, getMouseWheelDelta), asCALL_THISCALL_ASGLOBAL, &Input::InputManager); assert(r >= 0);
            r = eng->RegisterGlobalFunction("int get_WheelPos()", asMETHOD(Input::InputMan, getMouseWheelPos), asCALL_THISCALL_ASGLOBAL, &Input::InputManager); assert(r >= 0);

            r = eng->RegisterGlobalFunction("bool IsPressed(Button)", asFUNCTION(sf::Mouse::isButtonPressed), asCALL_CDECL); assert(r >= 0);

            r = eng->SetDefaultNamespace(""); assert(r >= 0);
        });

        return true;
    }
}

bool Script::SFML::Extensions::Mouse = Reg();

