#pragma once

#include "Extensions.hpp"
#include <Input/InputManager.hpp>
#include <Script/ScriptExtensions.hpp>

#include <SFML/Window/Joystick.hpp>

#include <angelscript.h>

#include <cassert>

namespace
{
    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng){
            int r = 0;

            r = eng->SetDefaultNamespace("Joystick"); assert(r >= 0);

            r = eng->RegisterObjectType("Identifier", sizeof(sf::Joystick::Identification), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<sf::Joystick::Identification>()); assert(r >= 0);
            r = eng->RegisterObjectProperty("Identifier", "string Name", asOFFSET(sf::Joystick::Identification, name)); assert(r >= 0);
            r = eng->RegisterObjectProperty("Identifier", "uint Vendor", asOFFSET(sf::Joystick::Identification, vendorId)); assert(r >= 0);
            r = eng->RegisterObjectProperty("Identifier", "uint Product", asOFFSET(sf::Joystick::Identification, productId)); assert(r >= 0);


            r = eng->RegisterEnum("Axis"); assert(r >= 0);
            r = eng->RegisterEnumValue("Axis", "X", sf::Joystick::X); assert(r >= 0);
            r = eng->RegisterEnumValue("Axis", "Y", sf::Joystick::Y); assert(r >= 0);
            r = eng->RegisterEnumValue("Axis", "Z", sf::Joystick::Z); assert(r >= 0);
            r = eng->RegisterEnumValue("Axis", "R", sf::Joystick::R); assert(r >= 0);
            r = eng->RegisterEnumValue("Axis", "U", sf::Joystick::U); assert(r >= 0);
            r = eng->RegisterEnumValue("Axis", "V", sf::Joystick::V); assert(r >= 0);
            r = eng->RegisterEnumValue("Axis", "PovX", sf::Joystick::PovX); assert(r >= 0);
            r = eng->RegisterEnumValue("Axis", "PovY", sf::Joystick::PovY); assert(r >= 0);


            r = eng->RegisterGlobalFunction("bool Connected(uint id)", asFUNCTION(sf::Joystick::isConnected), asCALL_CDECL); assert(r >= 0);
            r = eng->RegisterGlobalFunction("uint ButtonCount(uint id)", asFUNCTION(sf::Joystick::getButtonCount), asCALL_CDECL); assert(r >= 0);
            r = eng->RegisterGlobalFunction("bool HasAxis(uint id, Axis)", asFUNCTION(sf::Joystick::hasAxis), asCALL_CDECL); assert(r >= 0);
            r = eng->RegisterGlobalFunction("bool IsPressed(uint id, uint button)", asFUNCTION(sf::Joystick::isButtonPressed), asCALL_CDECL); assert(r >= 0);
            r = eng->RegisterGlobalFunction("float AxisPosition(uint id, Axis)", asFUNCTION(sf::Joystick::getAxisPosition), asCALL_CDECL); assert(r >= 0);
            r = eng->RegisterGlobalFunction("Identifier Identification(uint id)", asFUNCTION(sf::Joystick::getIdentification), asCALL_CDECL); assert(r >= 0);

            r = eng->SetDefaultNamespace(""); assert(r >= 0);
        });

        return true;
    }
}

bool Script::SFML::Extensions::Joystick = Reg();

