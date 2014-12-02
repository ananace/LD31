#include <Input/InputManager.hpp>
#include <Script/ScriptExtensions.hpp>
#include <Script/SFML/Extensions.hpp>

#include <angelscript.h>

#include <iostream>
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

void ExceptionCallback(asIScriptContext *ctx, void* /*param*/)
{
    std::cout << "!! Script Exception occured !!\n" << std::endl;

    const asIScriptFunction *function = ctx->GetExceptionFunction();
    if (function->GetModule())
        std::cout << function->GetModuleName();
    else
        std::cout << function->GetScriptSectionName();
    int col = 0;
    std::cout << ":" << ctx->GetExceptionLineNumber(&col) << ":" << col << " in " << function->GetDeclaration() << ":" << std::endl;

    std::cout << ctx->GetExceptionString() << std::endl;

    asUINT callStackSize = ctx->GetCallstackSize();
    if (callStackSize > 1)
    {
        std::cout << "--- call stack ---" << std::endl;
        for (asUINT n = 1; n < callStackSize; n++)
        {
            function = ctx->GetFunction(n);
            if (function)
            {
                if (function->GetFuncType() == asFUNC_SCRIPT)
                {
                    if (function->GetModule())
                        std::cout << function->GetModuleName();
                    else
                        std::cout << function->GetScriptSectionName();
                    int col = 0;
                    std::cout << ":" << ctx->GetLineNumber(n, &col) << ":" << col << " in " << function->GetDeclaration() << std::endl;
                }
                else
                {
                    // The context is being reused by the application for a nested call
                    std::cout << "{...application...}: " << function->GetDeclaration() << std::endl;
                }
            }
            else
            {
                // The context is being reused by the script engine for a nested call
                std::cout << "{...script engine...}" << std::endl;
            }
        }
    }

    std::cout << std::endl;
}

asIScriptContext* getContext(asIScriptEngine* eng, void*)
{
    auto* ctx = eng->CreateContext();
    ctx->SetExceptionCallback(asFUNCTION(ExceptionCallback), nullptr, asCALL_CDECL);

    return ctx;
}

void returnContext(asIScriptEngine* eng, asIScriptContext* ctx, void*)
{
    ctx->Release();
}

enum Inputs
{
    Input_Up,
    Input_Down,
    Input_MAX
};

int main(int argc, char** argv)
{
    asIScriptEngine* engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

    engine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);
    engine->SetContextCallbacks(getContext, returnContext);

    // Make certain that the scripts registered correctly
    assert(Script::ScriptExtensions::RegisteredCommonExtensions());
    assert(Script::SFML::RegisteredExtensions());

    // Register the extensions into the engine
    Script::ScriptExtensions::RegisterAll(engine);

    Input::InputManager.setBindCount(Input_MAX);
    Input::InputManager.linkInputs(Input_Up, Input_Down);

    Input::InputManager.bindInput(Input_Up, Input::Input::Bind{ Input::Input::Bind::Bind_Keyboard, { { sf::Keyboard::W, 0 } } });
    // Input::InputManager.bindInput(Input_Down, Input::Input::Bind{ Input::Input::Bind::Bind_Keyboard, { { sf::Keyboard::S, 0 } } }); // Linking does this automagically

    // TODO: A Game


    engine->Release();
    return 0;
}
