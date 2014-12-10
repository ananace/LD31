#include "Extensions.hpp"
#include "Extensions.inl"
#include <Math/Rect.hpp>
#include <Math/Vector.hpp>
#include <Script/ScriptExtensions.hpp>
#include <Util/ResourceManager.hpp>

#include <Game/Application.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Text.hpp>

#include <SFML/Window/Mouse.hpp>

#include <angelscript.h>

#include <iostream>
#include <cassert>

namespace
{
    typedef Util::Resource<sf::Shader, std::string> Shader_t;

    sf::View* getDefaultView(sf::RenderTarget* target)
    {
        return const_cast<sf::View*>(&target->getDefaultView());
    }
    sf::View* getView(sf::RenderTarget* target)
    {
        return const_cast<sf::View*>(&target->getView());
    }
    void setView(sf::View* view, sf::RenderTarget* target)
    {
        target->setView(*view);
    }
#ifdef AS_SUPPORT_VALRET
    Math::Vector2 getSize(sf::RenderTarget* target)
    {
        return target->getSize();
    }
    Math::Rect getViewport(sf::View* view, sf::RenderTarget* target)
    {
        return target->getViewport(*view);
    }
    Math::Vector2 mapCoordsToPixel(const Math::Vector2& vec, sf::RenderTarget* target)
    {
        return target->mapCoordsToPixel(vec);
    }
    Math::Vector2 mapCoordsToPixel_view(const Math::Vector2& vec, sf::View* view, sf::RenderTarget* target)
    {
        return target->mapCoordsToPixel(vec, *view);
    }
    Math::Vector2 mapPixelToCoords(const Math::Vector2& vec, sf::RenderTarget* target)
    {
        return target->mapPixelToCoords(vec);
    }
    Math::Vector2 mapPixelToCoords_view(const Math::Vector2& vec, sf::View* view, sf::RenderTarget* target)
    {
        return target->mapPixelToCoords(vec, *view);
    }
#else
    void getSize(asIScriptGeneric* gen)
    {
        sf::RenderTarget* target = reinterpret_cast<sf::RenderTarget*>(gen->GetObject());
        new (gen->GetAddressOfReturnLocation()) Math::Vector2(target->getSize());
    }
    void getViewport(asIScriptGeneric* gen)
    {
        sf::RenderTarget* target = reinterpret_cast<sf::RenderTarget*>(gen->GetObject());
        sf::View* view = reinterpret_cast<sf::View*>(gen->GetArgObject(0));
        new (gen->GetAddressOfReturnLocation()) Math::Rect(target->getViewport(*view));
    }
    void mapCoordsToPixel(asIScriptGeneric* gen)
    {
        sf::RenderTarget* target = reinterpret_cast<sf::RenderTarget*>(gen->GetObject());
        Math::Vector2 coords = *reinterpret_cast<Math::Vector2*>(gen->GetArgObject(0));
        new (gen->GetAddressOfReturnLocation()) Math::Vector2(target->mapCoordsToPixel(coords));
    }
    void mapCoordsToPixel_view(asIScriptGeneric* gen)
    {
        sf::RenderTarget* target = reinterpret_cast<sf::RenderTarget*>(gen->GetObject());
        Math::Vector2 coords = *reinterpret_cast<Math::Vector2*>(gen->GetArgObject(0));
        sf::View* view = reinterpret_cast<sf::View*>(gen->GetArgObject(1));
        new (gen->GetAddressOfReturnLocation()) Math::Vector2(target->mapCoordsToPixel(coords, *view));
    }
    void mapPixelToCoords(asIScriptGeneric* gen)
    {
        sf::RenderTarget* target = reinterpret_cast<sf::RenderTarget*>(gen->GetObject());
        Math::Vector2 pixel = *reinterpret_cast<Math::Vector2*>(gen->GetArgObject(0));
        new (gen->GetAddressOfReturnLocation()) Math::Vector2(target->mapPixelToCoords(pixel));
    }
    void mapPixelToCoords_view(asIScriptGeneric* gen)
    {
        sf::RenderTarget* target = reinterpret_cast<sf::RenderTarget*>(gen->GetObject());
        Math::Vector2 pixel = *reinterpret_cast<Math::Vector2*>(gen->GetArgObject(0));
        sf::View* view = reinterpret_cast<sf::View*>(gen->GetArgObject(1));
        new (gen->GetAddressOfReturnLocation()) Math::Vector2(target->mapPixelToCoords(pixel, *view));
    }
#endif

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;

            r = eng->RegisterObjectType("Renderer", 0, asOBJ_REF | asOBJ_NOCOUNT);

#ifdef AS_SUPPORT_VALRET
            r = eng->RegisterObjectMethod("Renderer", "Vec2 get_Size()", asFUNCTION(getSize), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "Vec2 CoordsToPixel(Vec2&in)", asFUNCTION(mapCoordsToPixel), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "Vec2 CoordsToPixel(Vec2&in,View@)", asFUNCTION(mapCoordsToPixel_view), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "Vec2 PixelToCoords(Vec2&in)", asFUNCTION(mapPixelToCoords), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "Vec2 PixelToCoords(Vec2&in,View@)", asFUNCTION(mapPixelToCoords_view), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "Rect GetViewport(View@)", asFUNCTION(getViewport), asCALL_CDECL_OBJLAST); assert(r >= 0);
#else
            r = eng->RegisterObjectMethod("Renderer", "Vec2 get_Size()", asFUNCTION(getSize), asCALL_GENERIC); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "Vec2 CoordsToPixel(Vec2&in)", asFUNCTION(mapCoordsToPixel), asCALL_GENERIC); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "Vec2 CoordsToPixel(Vec2&in,View@)", asFUNCTION(mapCoordsToPixel_view), asCALL_GENERIC); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "Vec2 PixelToCoords(Vec2&in)", asFUNCTION(mapPixelToCoords), asCALL_GENERIC); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "Vec2 PixelToCoords(Vec2&in,View@)", asFUNCTION(mapPixelToCoords_view), asCALL_GENERIC); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "Rect GetViewport(View@)", asFUNCTION(getViewport), asCALL_GENERIC); assert(r >= 0);
#endif

            r = eng->RegisterObjectMethod("Renderer", "View@ get_DefaultView()", asFUNCTION(getDefaultView), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "View@ get_View()", asFUNCTION(getView), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "void set_View(View@)", asFUNCTION(setView), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->RegisterObjectMethod("Renderer", "void Clear(Color&in = Colors::Black)", asMETHOD(sf::RenderTarget, clear), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "void Draw(Sprite&in)", asFUNCTION(drawShape<sf::Sprite>), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "void Draw(Text&in)", asFUNCTION(drawShape<sf::Text>), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "void Draw(Sprite&in,Shader@)", asFUNCTION(drawShape_shader<sf::Sprite>), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "void Draw(Text&in,Shader@)", asFUNCTION(drawShape_shader<sf::Text>), asCALL_CDECL_OBJLAST); assert(r >= 0);
        }, 5);

        return true;
    }

}

bool Script::SFML::Extensions::RenderTarget = Reg();
