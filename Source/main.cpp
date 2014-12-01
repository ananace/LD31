#include <Math/Common.hpp>
#include <Math/Vector.hpp>
#include <Math/Rect.hpp>
#include <Script/ScriptExtensions.hpp>

#include <angelscript.h>
#include <cassert>

int main(int argc, char** argv)
{
    asIScriptEngine* engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

    // Make certain that the scripts registered correctly
    assert(Script::ScriptExtensions::CommonExtensions);
    assert(Math::CommonMathScriptRegistered);
    assert(Vector2::ScriptRegistered);
    assert(Rect::ScriptRegistered);

    // Register the extensions into the engine
    Script::ScriptExtensions::RegisterAll(engine);

    ///\TODO: Game class

    return 0;
}
