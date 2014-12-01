#include "Extensions.hpp"
#include <Math/Rect.hpp>
#include <Math/Vector.hpp>
#include <Script/ScriptExtensions.hpp>
#include <Util/ResourceManager.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <angelscript.h>

#include <cassert>

namespace
{
    typedef Util::Resource<sf::Shader, std::string> Shader_t;

    void draw(const sf::Drawable& draw, Shader_t* shader, sf::RenderTarget& target)
    {
        target.draw(draw, &(**shader));
    }

    Math::Vector2 getSize(sf::RenderTarget& target)
    {
        return target.getSize();
    }
    Math::Rect getViewport(const sf::View& view, sf::RenderTarget& target)
    {
        return target.getViewport(view);
    }
    Math::Vector2 mapCoordsToPixel(const Math::Vector2& vec, sf::RenderTarget& target)
    {
        return target.mapCoordsToPixel(vec);
    }
    Math::Vector2 mapCoordsToPixel_view(const Math::Vector2& vec, const sf::View& view, sf::RenderTarget& target)
    {
        return target.mapCoordsToPixel(vec, view);
    }
    Math::Vector2 mapPixelToCoords(const Math::Vector2& vec, sf::RenderTarget& target)
    {
        return target.mapPixelToCoords(vec);
    }
    Math::Vector2 mapPixelToCoords_view(const Math::Vector2& vec, const sf::View& view, sf::RenderTarget& target)
    {
        return target.mapPixelToCoords(vec, view);
    }

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;

            r = eng->RegisterObjectType("Renderer", 0, asOBJ_REF | asOBJ_NOCOUNT);

            r = eng->RegisterObjectMethod("Renderer", "View& get_DefaultView()", asMETHOD(sf::RenderTarget, getDefaultView), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "Vec2 get_Size()", asFUNCTION(getSize), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "View& get_View()", asMETHOD(sf::RenderTarget, getView), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "void set_View(View&in)", asMETHOD(sf::RenderTarget, setView), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Renderer", "Rect GetViewport(View&in)", asFUNCTION(getViewport), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "Vec2 MapCoordsToPixel(Vec2&in)", asFUNCTION(mapCoordsToPixel), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "Vec2 MapCoordsToPixel(Vec2&in,View&in)", asFUNCTION(mapCoordsToPixel_view), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "Vec2 mapPixelToCoords(Vec2&in)", asFUNCTION(mapPixelToCoords), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "Vec2 mapPixelToCoords(Vec2&in,View&in)", asFUNCTION(mapPixelToCoords_view), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->RegisterObjectMethod("Renderer", "void Clear(Color&in)", asMETHOD(sf::RenderTarget, clear), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Renderer", "void Draw(Sprite&in)", asMETHODPR(sf::RenderTarget, draw, (const sf::Drawable&, const sf::RenderStates&), void), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "void Draw(Text&in)", asMETHODPR(sf::RenderTarget, draw, (const sf::Drawable&, const sf::RenderStates&), void), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Renderer", "void Draw(Sprite&in,Shader@)", asFUNCTION(draw), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Renderer", "void Draw(Text&in,Shader@)", asFUNCTION(draw), asCALL_CDECL_OBJLAST); assert(r >= 0);
        }, 5);

        return true;
    }

}

bool Script::SFML::Extensions::RenderTarget = Reg();
