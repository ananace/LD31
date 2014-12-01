#include "Extensions.hpp"
#include "Extensions.inl"
#include <Script/ScriptExtensions.hpp>
#include <Util/FontFinder.hpp>

#include <SFML/Graphics/Font.hpp>

#include <angelscript.h>

#include <cassert>

namespace
{
    sf::Font* getDefaultFont()
    {
        return &Util::FontFinder::getDefaultFont();
    }

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng){
            int r = 0;

            r = eng->RegisterObjectType("Font", 0, asOBJ_REF | asOBJ_NOCOUNT); assert(r >= 0);

            r = eng->SetDefaultNamespace("Fonts"); assert(r >= 0);
            r = eng->RegisterGlobalFunction("Font@ get_DefaultFont()", asFUNCTION(getDefaultFont), asCALL_CDECL);
            r = eng->SetDefaultNamespace(""); assert(r >= 0);
        });

        return true;
    }

}

bool Script::SFML::Extensions::Font = Reg();
