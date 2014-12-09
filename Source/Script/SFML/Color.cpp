#include "Extensions.hpp"
#include "Extensions.inl"
#include <Script/ScriptExtensions.hpp>

#include <SFML/Graphics/Color.hpp>

#include <angelscript.h>

#include <cassert>

namespace
{
    void create_color(sf::Color* memory)
    {
        new (memory)sf::Color();
    }

    void create_color_val(uint8_t r, uint8_t g, uint8_t b, uint8_t a, sf::Color* memory)
    {
        new (memory)sf::Color(r, g, b, a);
    }
    void destruct_color(sf::Color* memory)
    {
        memory->~Color();
    }

    bool color_equals(const sf::Color& rhs, const sf::Color& lhs)
    {
        return lhs == rhs;
    }
#ifndef AS_SUPPORT_VALRET
    void color_add(asIScriptGeneric* gen)
    {
        sf::Color& lhs = *reinterpret_cast<sf::Color*>(gen->GetObject());
        sf::Color& rhs = *reinterpret_cast<sf::Color*>(gen->GetArgObject(0));
        new (gen->GetAddressOfReturnLocation()) sf::Color(lhs + rhs);
    }
    void color_mul(asIScriptGeneric* gen)
    {
        sf::Color& lhs = *reinterpret_cast<sf::Color*>(gen->GetObject());
        sf::Color& rhs = *reinterpret_cast<sf::Color*>(gen->GetArgObject(0));
        new (gen->GetAddressOfReturnLocation()) sf::Color(lhs * rhs);
    }
    void color_sub(asIScriptGeneric* gen)
    {
        sf::Color& lhs = *reinterpret_cast<sf::Color*>(gen->GetObject());
        sf::Color& rhs = *reinterpret_cast<sf::Color*>(gen->GetArgObject(0));
        new (gen->GetAddressOfReturnLocation()) sf::Color(lhs - rhs);
    }
#endif

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng){
            int r = 0;

            r = eng->RegisterObjectType("Color", sizeof(sf::Color), asOBJ_VALUE | asGetTypeTraits<sf::Color>()); assert(r >= 0);

            r = eng->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(create_color), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f(uint8,uint8,uint8,uint8=255)", asFUNCTION(create_color_val), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Color", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destruct_color), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->RegisterObjectMethod("Color", "Color& opAssign(Color&in)", asMETHODPR(sf::Color, operator=, (const sf::Color&), sf::Color&), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Color", "Color& opAddAssign(Color&in)", asFUNCTIONPR(sf::operator+=, (sf::Color&, const sf::Color&), sf::Color&), asCALL_CDECL_OBJFIRST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Color", "Color& opSubAssign(Color&in)", asFUNCTIONPR(sf::operator-=, (sf::Color&, const sf::Color&), sf::Color&), asCALL_CDECL_OBJFIRST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Color", "Color& opMulAssign(Color&in)", asFUNCTIONPR(sf::operator*=, (sf::Color&, const sf::Color&), sf::Color&), asCALL_CDECL_OBJFIRST); assert(r >= 0);

#ifdef AS_SUPPORT_VALRET
            r = eng->RegisterObjectMethod("Color", "Color opAdd(Color&in)", asFUNCTIONPR(sf::operator+, (const sf::Color&, const sf::Color&), sf::Color), asCALL_CDECL_OBJFIRST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Color", "Color opSub(Color&in)", asFUNCTIONPR(sf::operator-, (const sf::Color&, const sf::Color&), sf::Color), asCALL_CDECL_OBJFIRST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Color", "Color opMul(Color&in)", asFUNCTIONPR(sf::operator*, (const sf::Color&, const sf::Color&), sf::Color), asCALL_CDECL_OBJFIRST); assert(r >= 0);
#else
            r = eng->RegisterObjectMethod("Color", "Color opAdd(Color&in)", asFUNCTION(color_add), asCALL_GENERIC); assert(r >= 0);
            r = eng->RegisterObjectMethod("Color", "Color opSub(Color&in)", asFUNCTION(color_sub), asCALL_GENERIC); assert(r >= 0);
            r = eng->RegisterObjectMethod("Color", "Color opMul(Color&in)", asFUNCTION(color_mul), asCALL_GENERIC); assert(r >= 0);
#endif

            r = eng->RegisterObjectMethod("Color", "bool opEquals(Color&in)", asFUNCTIONPR(sf::operator==, (const sf::Color&, const sf::Color&), bool), asCALL_CDECL_OBJFIRST); assert(r >= 0);

            r = eng->RegisterObjectProperty("Color", "uint8 R", asOFFSET(sf::Color, r)); assert(r >= 0);
            r = eng->RegisterObjectProperty("Color", "uint8 G", asOFFSET(sf::Color, g)); assert(r >= 0);
            r = eng->RegisterObjectProperty("Color", "uint8 B", asOFFSET(sf::Color, b)); assert(r >= 0);
            r = eng->RegisterObjectProperty("Color", "uint8 A", asOFFSET(sf::Color, a)); assert(r >= 0);

            r = eng->SetDefaultNamespace("Colors"); assert(r >= 0);

            r = eng->RegisterGlobalProperty("const Color Black", const_cast<sf::Color*>(&sf::Color::Black));
            r = eng->RegisterGlobalProperty("const Color Blue", const_cast<sf::Color*>(&sf::Color::Blue));
            r = eng->RegisterGlobalProperty("const Color Cyan", const_cast<sf::Color*>(&sf::Color::Cyan));
            r = eng->RegisterGlobalProperty("const Color Green", const_cast<sf::Color*>(&sf::Color::Green));
            r = eng->RegisterGlobalProperty("const Color Magenta", const_cast<sf::Color*>(&sf::Color::Magenta));
            r = eng->RegisterGlobalProperty("const Color Red", const_cast<sf::Color*>(&sf::Color::Red));
            r = eng->RegisterGlobalProperty("const Color Transparent", const_cast<sf::Color*>(&sf::Color::Transparent));
            r = eng->RegisterGlobalProperty("const Color White", const_cast<sf::Color*>(&sf::Color::White));
            r = eng->RegisterGlobalProperty("const Color Yellow", const_cast<sf::Color*>(&sf::Color::Yellow));

            r = eng->SetDefaultNamespace(""); assert(r >= 0);

            Script::ScriptManager.registerType("Color", []() {
                return new CSFMLType<sf::Color>();
            });
        });

        return true;
    }

}

bool Script::SFML::Extensions::Color = Reg();
