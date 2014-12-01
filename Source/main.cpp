#include <Script/ScriptExtensions.hpp>
#include <Script/SFML/Extensions.hpp>

#include <angelscript.h>
#include <cassert>

void MessageCallback(const asSMessageInfo *msg, void* /*param*/)
{
    const char *type = "ERR ";
    if (msg->type == asMSGTYPE_WARNING)
        type = "WARN";
    else if (msg->type == asMSGTYPE_INFORMATION)
        type = "INFO";

    printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

int main(int argc, char** argv)
{
    asIScriptEngine* engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

    engine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);

    // Make certain that the scripts registered correctly
    assert(Script::ScriptExtensions::RegisteredCommonExtensions());
    assert(Script::SFML::RegisteredExtensions());

    // Register the extensions into the engine
    Script::ScriptExtensions::RegisterAll(engine);

    ///\TODO: Game class


    engine->Release();
    return 0;
}
