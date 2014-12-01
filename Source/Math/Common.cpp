#include "Common.hpp"
#include "Spinor.hpp"
#include "Vector.hpp"

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

float Math::SlerpAngle(float start, float end, float delta)
{
    return Spinor(cos(start / 2), sin(start / 2)).slerp(Spinor(cos(end / 2), sin(end / 2)), delta).getAngle();
}

namespace
{
    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;
            r = eng->SetDefaultNamespace("Math"); assert(r >= 0);

            r = eng->RegisterGlobalProperty("const float PI", (void*)&Math::PI);
            r = eng->RegisterGlobalProperty("const float DOUBLE_PI", (void*)&Math::DOUBLE_PI);
            r = eng->RegisterGlobalProperty("const float HALF_PI", (void*)&Math::HALF_PI);
            r = eng->RegisterGlobalProperty("const float PHI", (void*)&Math::PHI);

            r = eng->RegisterGlobalProperty("const float R2D", (void*)&Math::Rad2Deg);
            r = eng->RegisterGlobalProperty("const float D2R", (void*)&Math::Deg2Rad);

            r = eng->RegisterGlobalFunction("bool FloatCompare(float,float,float)", asFUNCTION(Math::FloatCompare), asCALL_CDECL); assert(r >= 0);
            
            r = eng->RegisterGlobalFunction("int Lerp(int&in,int&in,float)", asFUNCTION(Math::Lerp<int>), asCALL_CDECL); assert(r >= 0);
            r = eng->RegisterGlobalFunction("float Lerp(float&in,float&in,float)", asFUNCTION(Math::Lerp<float>), asCALL_CDECL); assert(r >= 0);
            r = eng->RegisterGlobalFunction("Vec2 Lerp(Vec2&in,Vec2&in,float)", asFUNCTION(Math::Lerp<Vector2>), asCALL_CDECL); assert(r >= 0);
            // r = eng->RegisterGlobalFunction("Spinor Lerp(Spinor&in,Spinor&in,float)", asFUNCTION(Math::Lerp<Spinor>), asCALL_CDECL); assert(r >= 0);

            r = eng->RegisterGlobalFunction("float SlerpAngle(float,float,float)", asFUNCTION(Math::SlerpAngle), asCALL_CDECL); assert(r >= 0);

            r = eng->SetDefaultNamespace(""); assert(r >= 0);
        });

        return true;
    }
}

bool Script::ScriptExtensions::CommonMath = Reg();
