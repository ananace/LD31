#include "Common.hpp"
#include <Script/ScriptExtensions.hpp>

#include <angelscript.h>

#include <cassert>
#include <cmath>

using namespace Math;

bool Math::FloatCompare(const float a, const float b, const float EPSILON)
{
    assert(EPSILON > 0 && "Epsilon value must be positive");

    return abs(a - b) < EPSILON;
}

namespace
{
    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;
            r = eng->SetDefaultNamespace("Math"); assert(r >= 0);

            r = eng->RegisterGlobalFunction("bool FloatCompare(float,float,float)", asFUNCTION(Math::FloatCompare), asCALL_CDECL); assert(r >= 0);

            r = eng->SetDefaultNamespace(""); assert(r >= 0);
        });

        return true;
    }
}

bool Math::CommonMathScriptRegistered = Reg();
