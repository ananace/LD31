#include <Application.hpp>
#include <Input/InputManager.hpp>
#include <Script/ScriptExtensions.hpp>
#include <Util/FileSystem.hpp>

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
    std::cout << "!! Script Exception occured !!" << std::endl;

    const asIScriptFunction *function = ctx->GetExceptionFunction();
    int col = 0;
    std::cout << function->GetScriptSectionName() << ":" << ctx->GetExceptionLineNumber(&col) << ":" << col << " in " << function->GetDeclaration() << ":" << std::endl;

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
                    int col = 0;
                    std::cout << function->GetScriptSectionName() << ":" << ctx->GetLineNumber(n, &col) << ":" << col << " in " << function->GetDeclaration() << std::endl;
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

#ifdef LD31_WINDOWS
#include <Windows.h>
#endif

int main(int argc, char** argv)
{
    // Run program from it's executable directory
    Util::FileSystem::setSaneWorkingDir(argv[0]);
    
    asIScriptEngine* engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

    engine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);
    engine->SetContextCallbacks(getContext, returnContext);

    // Make certain that the scripts registered correctly
    assert(Script::ScriptExtensions::RegisteredAllExtensions());

    // Register the extensions into the engine
    Script::ScriptExtensions::RegisterAll(engine);

    Application app(engine);

    app.runGameLoop();

    engine->Release();
    return 0;
}
