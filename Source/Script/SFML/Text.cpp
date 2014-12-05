#include "Extensions.hpp"
#include "Extensions.inl"
#include <Math/Rect.hpp>
#include <Math/Vector.hpp>
#include <Script/ScriptExtensions.hpp>
#include <Util/FontFinder.hpp>

#include <SFML/Graphics/Text.hpp>

#include <angelscript.h>

#include <cassert>

void create_text(void* memory)
{
    new (memory)sf::Text("", Util::FontFinder::getDefaultFont());
}
void create_text_string(const std::string& string, void* memory)
{
    new (memory)sf::Text(string, Util::FontFinder::getDefaultFont());
}
void create_text_stringFont(const std::string& string, sf::Font* font, void* memory)
{
    new (memory)sf::Text(string, *font);
}
void create_text_stringFontSize(const std::string& string, sf::Font* font, unsigned int size, void* memory)
{
    new (memory)sf::Text(string, *font, size);
}
void destruct_text(sf::Text* text)
{
    text->~Text();
}

Math::Vector2 getCharacterPos(size_t index, sf::Text& text)
{
    return text.findCharacterPos(index);
}
sf::Font* getFont(sf::Text& text)
{
    return const_cast<sf::Font*>(text.getFont());
}
void setFont(sf::Font* font, sf::Text& text)
{
    text.setFont(*font);
}
Math::Rect getGlobalBounds(sf::Text& text)
{
    return text.getGlobalBounds();
}
Math::Rect getLocalBounds(sf::Text& text)
{
    return text.getLocalBounds();
}

std::string getString(sf::Text& text)
{
    return text.getString();
}
void setString(const std::string& str, sf::Text& text)
{
    text.setString(str);
}

namespace
{

    bool Reg()
    {
        Script::ScriptExtensions::AddExtension([](asIScriptEngine* eng) {
            int r = 0;

            r = eng->SetDefaultNamespace("Text"); assert(r >= 0);

            r = eng->RegisterEnum("Style"); assert(r >= 0);
            r = eng->RegisterEnumValue("Style", "Regular", sf::Text::Regular); assert(r >= 0);
            r = eng->RegisterEnumValue("Style", "Bold", sf::Text::Bold); assert(r >= 0);
            r = eng->RegisterEnumValue("Style", "Italic", sf::Text::Italic); assert(r >= 0);
            r = eng->RegisterEnumValue("Style", "Underline", sf::Text::Underlined); assert(r >= 0);
            r = eng->RegisterEnumValue("Style", "Strikethrough", sf::Text::StrikeThrough); assert(r >= 0);

            r = eng->SetDefaultNamespace(""); assert(r >= 0);

            r = eng->RegisterObjectType("Text", sizeof(sf::Text), asOBJ_VALUE | asGetTypeTraits<sf::Text>()); assert(r >= 0);

            r = eng->RegisterObjectBehaviour("Text", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(create_text), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Text", asBEHAVE_CONSTRUCT, "void f(string&in)", asFUNCTION(create_text_string), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Text", asBEHAVE_CONSTRUCT, "void f(string&in,Font@)", asFUNCTION(create_text_stringFont), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Text", asBEHAVE_CONSTRUCT, "void f(string&in,Font@,uint)", asFUNCTION(create_text_stringFontSize), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectBehaviour("Text", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destruct_text), asCALL_CDECL_OBJLAST); assert(r >= 0);

            r = eng->RegisterObjectMethod("Text", "Text& opAssign(Text&in)", asMETHODPR(sf::Text, operator=, (const sf::Text&), sf::Text&), asCALL_THISCALL); assert(r >= 0);

            r = eng->RegisterObjectMethod("Text", "uint get_CharacterSize()", asMETHOD(sf::Text, getCharacterSize), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Text", "void set_CharacterSize(uint)", asMETHOD(sf::Text, setCharacterSize), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Text", "Color get_Color()", asMETHOD(sf::Text, getColor), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Text", "void set_Color(Color&in)", asMETHOD(sf::Text, setColor), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Text", "Rect get_GlobalBounds()", asFUNCTION(getGlobalBounds), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Text", "Rect get_LocalBounds()", asFUNCTION(getGlobalBounds), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Text", "string get_String()", asFUNCTION(getString), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Text", "void set_String(string&in)", asFUNCTION(setString), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Text", "uint get_Style()", asMETHOD(sf::Text, getStyle), asCALL_THISCALL); assert(r >= 0);
            r = eng->RegisterObjectMethod("Text", "void set_Style(uint)", asMETHOD(sf::Text, setStyle), asCALL_THISCALL); assert(r >= 0);
            
            r = eng->RegisterObjectMethod("Text", "Vec2 CharacterPos(uint64)", asFUNCTION(getCharacterPos), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Text", "Font@ GetFont()", asFUNCTION(getFont), asCALL_CDECL_OBJLAST); assert(r >= 0);
            r = eng->RegisterObjectMethod("Text", "void SetFont(Font@)", asFUNCTION(setFont), asCALL_CDECL_OBJLAST); assert(r >= 0);

            Script::SFML::registerTransformable<sf::Text>("Text", eng);
        });

        return true;
    }

}

bool Script::SFML::Extensions::Text = Reg();
