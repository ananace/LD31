#include "Extensions.hpp"
#include "Extensions.inl"
#include <Math/Rect.hpp>
#include <Math/Vector.hpp>
#include <Script/ScriptExtensions.hpp>
#include <Util/ResourceManager.hpp>

#include <SFML/Graphics/Sprite.hpp>

#include <angelscript.h>

#include <cassert>

namespace
{
    typedef Util::Resource<sf::Texture, std::string> Texture_t;

    std::unordered_map<void*, Texture_t*> sprites;

    void create_Sprite(void* memory)
    {
        new(memory)sf::Sprite();
    }
    void create_Sprite_tex(Texture_t* texture, void* memory)
    {
        new(memory)sf::Sprite(**texture);

        sprites[memory] = texture;
    }
    void create_Sprite_texRect(Texture_t* texture, const Math::Rect& rect, void* memory)
    {
        new(memory)sf::Sprite(**texture, rect);

        sprites[memory] = texture;
    }
    void destruct_Sprite(sf::Sprite* sprite)
    {
        sprite->~Sprite();

        if (sprites.count(sprite) == 0)
            return;

        sprites[sprite]->GCRelease();
        sprites.erase(sprite);
    }

    sf::Sprite& assign_Sprite(const sf::Sprite& rhs, sf::Sprite& lhs)
    {
        if (sprites.count(const_cast<sf::Sprite*>(&rhs)) > 0)
            sprites[const_cast<sf::Sprite*>(&rhs)]->GCAddRef();
        if (sprites.count(&lhs) > 0)
            sprites[&lhs]->GCRelease();

        lhs = rhs;

        if (sprites.count(const_cast<sf::Sprite*>(&rhs)) > 0)
            sprites[&lhs] = sprites[const_cast<sf::Sprite*>(&rhs)];

        return lhs;
    }

    Texture_t* getSpriteTexture(sf::Sprite& sprite)
    {
        if (sprites.count(&sprite) == 0)
            return nullptr;

        sprites[&sprite]->GCAddRef();
        return sprites[&sprite];
    }
    void setSpriteTextureReset(Texture_t* texture, bool resetRect, sf::Sprite& sprite)
    {
        if (!texture)
            return;

        if (sprites.count(&sprite) > 0)
            sprites[&sprite]->GCRelease();

        sprite.setTexture(**texture, resetRect);

        texture->GCAddRef();
        sprites[&sprite] = texture;
    }

    void setSpriteTextureRect(const Math::Rect& rect, sf::Sprite& sprite)
    {
        sprite.setTextureRect(rect);
    }

#ifndef AS_SUPPORT_VALRET
    void getSpriteColor(asIScriptGeneric* gen)
    {
        sf::Sprite& text = *reinterpret_cast<sf::Sprite*>(gen->GetObject());
        new (gen->GetAddressOfReturnLocation()) sf::Color(text.getColor());
    }
#endif

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;

            r = eng->RegisterObjectType("Sprite", sizeof(sf::Sprite), asOBJ_VALUE | asGetTypeTraits<sf::Sprite>()); assert(r >= 0);

#ifdef AS_SUPPORT_VALRET
            r = eng->RegisterObjectMethod("Sprite", "Rect get_GlobalBounds()", asFUNCTION(getGlobalBounds<sf::Sprite>), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Sprite", "Rect get_LocalBounds()", asFUNCTION(getLocalBounds<sf::Sprite>), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Sprite", "Rect get_TextureRect()", asFUNCTION(getTextureRect<sf::Sprite>), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Sprite", "Color get_Color()", asMETHOD(sf::Sprite, getColor), asCALL_THISCALL); assert(r >= 0);
#else
            r = eng->RegisterObjectMethod("Sprite", "Rect get_GlobalBounds()", asFUNCTION(getGlobalBounds<sf::Sprite>), asCALL_GENERIC); assert(r >= 0);
            r = eng->RegisterObjectMethod("Sprite", "Rect get_LocalBounds()", asFUNCTION(getLocalBounds<sf::Sprite>), asCALL_GENERIC); assert(r >= 0);
            r = eng->RegisterObjectMethod("Sprite", "Rect get_TextureRect()", asFUNCTION(getTextureRect<sf::Sprite>), asCALL_GENERIC); assert(r >= 0);
            r = eng->RegisterObjectMethod("Sprite", "Color get_Color()", asFUNCTION(getSpriteColor), asCALL_GENERIC); assert(r >= 0);
#endif

            r = eng->RegisterObjectBehaviour("Sprite", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(create_Sprite), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Sprite", asBEHAVE_CONSTRUCT, "void f(Texture@)", asFUNCTION(create_Sprite_tex), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Sprite", asBEHAVE_CONSTRUCT, "void f(Texture@,Rect&in)", asFUNCTION(create_Sprite_texRect), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Sprite", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destruct_Sprite), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->RegisterObjectMethod("Sprite", "Sprite& opAssign(Sprite&in)", asFUNCTION(assign_Sprite), asCALL_CDECL_OBJLAST); assert(r >= 0);

            
            r = eng->RegisterObjectMethod("Sprite", "void set_Color(Color&in)", asMETHOD(sf::Sprite, setColor), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Sprite", "void set_TextureRect(Rect&in)", asFUNCTION(setSpriteTextureRect), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->RegisterObjectMethod("Sprite", "Texture@ GetTexture()", asFUNCTION(getSpriteTexture), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Sprite", "void SetTexture(Texture@,bool resetTextureRect=false)", asFUNCTION(setSpriteTextureReset), asCALL_CDECL_OBJLAST); assert(r >= 0);

            Script::SFML::registerTransformable<sf::Sprite>("Sprite", eng);
        }, 1);

        return true;
    }
}

bool Script::SFML::Extensions::Sprite = Reg();