#pragma once

#include <angelscript.h>

#include <stdexcept>
#include <cstdint>

namespace
{
    inline void setCTXArg(asIScriptContext*, uint32_t) { }

#define PRIMITIVE_ARG(Type, SetType) template<typename... Args> \
    inline void setCTXArg(asIScriptContext* ctx, uint32_t id, Type arg, Args... args) \
    { \
        ctx->SetArg ## SetType (id, arg); \
        setCTXArg(ctx, id + 1, args...); \
    } //

    PRIMITIVE_ARG(uint8_t, Byte)
    PRIMITIVE_ARG(uint16_t, Word)
    PRIMITIVE_ARG(uint32_t, DWord)
    PRIMITIVE_ARG(uint64_t, QWord)
    PRIMITIVE_ARG(int8_t, Byte)
    PRIMITIVE_ARG(int16_t, Word)
    PRIMITIVE_ARG(int32_t, DWord)
    PRIMITIVE_ARG(int64_t, QWord)
    PRIMITIVE_ARG(float, Float)
    PRIMITIVE_ARG(double, Double)

#undef PRIMITIVE_ARG

    template<typename T, typename... Args>
    inline void setCTXArg(asIScriptContext* ctx, uint32_t id, T* arg, Args... args)
    {
        ctx->SetArgObject(id, const_cast<T*>(arg));
        setCTXArg(ctx, id + 1, args...);
    }

    template<typename T, typename... Args>
    inline void setCTXArg(asIScriptContext* ctx, uint32_t id, const T& arg, Args... args)
    {
        ctx->SetArgObject(id, const_cast<T*>(&arg));
        setCTXArg(ctx, id + 1, args...);
    }
}

template<typename... Args>
void Script::ScriptHooks::execute(const std::string& name, asIScriptEngine* eng, Args... args)
{
    if (mBoundHooks.count(name) == 0)
        return;

    auto& hooks = mBoundHooks[name];
    auto* ctx = eng->RequestContext();

    for (auto& hook : hooks)
    {
        ctx->Prepare(hook.Function);
        ctx->SetObject(hook.Object);

        setCTXArg(ctx, 0, args...);

        ctx->Execute();
    }

    eng->ReturnContext(ctx);
}
