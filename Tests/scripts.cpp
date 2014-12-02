#include <catch.hpp>

#include <Math/Vector.hpp>
#include <Script/ScriptExtensions.hpp>
#include <Script/ScriptManager.hpp>
#include <Script/SFML/Extensions.hpp>

#include <SFML/Graphics/Color.hpp>

#include <angelscript.h>

void MessageCallback(const asSMessageInfo *msg, void* /*param*/)
{
    if (msg->type == asMSGTYPE_WARNING)
    {
        WARN((std::string(msg->section) + " : " + msg->message));
    }
    else if (msg->type == asMSGTYPE_INFORMATION)
    {
        INFO((std::string(msg->section) + " : " + msg->message));
    }
    else
    {
        FAIL((std::string(msg->section) + " : " + msg->message));
    }
}

TEST_CASE("Basic script", "[script]")
{
    asIScriptEngine* eng = asCreateScriptEngine(ANGELSCRIPT_VERSION);
    REQUIRE(eng);

    REQUIRE(eng->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL) >= 0);

    REQUIRE(Script::ScriptExtensions::RegisteredCommonExtensions());
    REQUIRE(Script::SFML::RegisteredExtensions());

    REQUIRE_NOTHROW(Script::ScriptExtensions::RegisterAll(eng));

    GIVEN("A script returning a string")
    {
        auto* mod = eng->GetModule("TestModule", asGM_ALWAYS_CREATE);
        std::string testScript = "string Test() { return \"I am working\"; }";
        REQUIRE(mod->AddScriptSection("TestScript", testScript.c_str(), testScript.size()) >= 0);
        REQUIRE(mod->Build() >= 0);

        THEN("The script returns the string properly")
        {
            auto* func = mod->GetFunctionByDecl("string Test()");
            REQUIRE(func);

            auto* ctx = eng->RequestContext();
            REQUIRE(ctx);
            REQUIRE(ctx->Prepare(func) >= 0);
            REQUIRE(ctx->Execute() >= 0);

            std::string* ret = (std::string*)ctx->GetAddressOfReturnValue();
            REQUIRE(*ret == "I am working");
        }
    }

    GIVEN("A script using math extensions")
    {
        auto* mod = eng->GetModule("TestModule", asGM_ALWAYS_CREATE);
        std::string testScript = "Vec2 Test(Vec2&in vector) { return vector.Perpendicular; }";
        REQUIRE(mod->AddScriptSection("TestScript", testScript.c_str(), testScript.size()) >= 0);
        REQUIRE(mod->Build() >= 0);

        THEN("The script can use a vector")
        {
            auto* func = mod->GetFunctionByDecl("Vec2 Test(Vec2&in)");
            REQUIRE(func);

            Math::Vector2 vec(15, 2);

            auto* ctx = eng->RequestContext();
            REQUIRE(ctx);
            REQUIRE(ctx->Prepare(func) >= 0);
            REQUIRE(ctx->SetArgAddress(0, &vec) >= 0);
            REQUIRE(ctx->Execute() >= 0);

            Math::Vector2* ret = (Math::Vector2*)ctx->GetAddressOfReturnValue();
            REQUIRE(*ret == vec.getPerpendicular());
        }
    }

    GIVEN("A script using basic SFML extensions")
    {
        auto* mod = eng->GetModule("TestModule", asGM_ALWAYS_CREATE);
        std::string testScript = "Color Test() { return Colors::Cyan; }";
        REQUIRE(mod->AddScriptSection("TestScript", testScript.c_str(), testScript.size()) >= 0);
        REQUIRE(mod->Build() >= 0);

        THEN("The script can use the basic extensions")
        {
            auto* func = mod->GetFunctionByDecl("Color Test()");
            REQUIRE(func);

            auto* ctx = eng->RequestContext();
            REQUIRE(ctx);
            REQUIRE(ctx->Prepare(func) >= 0);
            REQUIRE(ctx->Execute() >= 0);

            sf::Color* ret = (sf::Color*)ctx->GetAddressOfReturnValue();
            REQUIRE(*ret == sf::Color::Cyan);
        }
    }

    GIVEN("A managed script")
    {
        REQUIRE(Script::ScriptManager.loadScriptFromMemory("TestScript", "Color Test() { return Colors::Cyan; }"));
        auto* mod = eng->GetModule("TestScript");

        THEN("The script works")
        {
            auto* func = mod->GetFunctionByDecl("Color Test()");
            REQUIRE(func);

            auto* ctx = eng->RequestContext();
            REQUIRE(ctx);
            REQUIRE(ctx->Prepare(func) >= 0);
            REQUIRE(ctx->Execute() >= 0);

            sf::Color* ret = (sf::Color*)ctx->GetAddressOfReturnValue();
            CHECK(*ret == sf::Color::Cyan);

            AND_THEN("After reloading, the script still works")
            {
                REQUIRE(Script::ScriptManager.loadScriptFromMemory("TestScript", "Color Test() { return Colors::Magenta; }"));

                auto* func = mod->GetFunctionByDecl("Color Test()");
                REQUIRE(func);

                auto* ctx = eng->RequestContext();
                REQUIRE(ctx);
                REQUIRE(ctx->Prepare(func) >= 0);
                REQUIRE(ctx->Execute() >= 0);

                sf::Color* ret = (sf::Color*)ctx->GetAddressOfReturnValue();
                CHECK(*ret == sf::Color::Magenta);
            }
        }
    }

    REQUIRE(eng->Release() >= 0);
}