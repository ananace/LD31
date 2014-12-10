#pragma once

#include <Math/Rect.hpp>
#include <Math/Vector.hpp>
#include <Script/ScriptExtensions.hpp>
#include <Script/ScriptManager.hpp>
#include <Util/ResourceManager.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include <angelscript.h>

#include <serializer_fix/serializer.h>

#include <cassert>

typedef Util::Resource<sf::Texture, std::string> Texture_t;
typedef Util::Resource<sf::Shader, std::string> Shader_t;

namespace Script
{
    namespace SFML
    {
        extern std::unordered_map<void*, Texture_t*> shapes;
    }
}

namespace
{

template<typename T>
struct CSFMLType : public CUserType
{
    void Store(CSerializedValue *val, void *ptr)
    {
        val->SetUserData(new T(*(T*)ptr));
    }
    void Restore(CSerializedValue *val, void *ptr)
    {
        T *buffer = (T*)val->GetUserData();
        *(T*)ptr = *buffer;
    }
    void CleanupUserData(CSerializedValue *val)
    {
        T *buffer = (T*)val->GetUserData();
        delete buffer;
    }
};

using Script::SFML::shapes;

template<typename T>
void drawShape_shader(const T& draw, Shader_t* shader, sf::RenderWindow* target)
{
    target->draw(draw, &(**shader));
}

template<typename T>
void drawShape(const T& draw, sf::RenderWindow* target)
{
    target->draw(draw);
}

#ifdef AS_SUPPORT_VALRET
template<typename T>
Math::Rect getGlobalBounds(T& text)
{
    return text.getGlobalBounds();
}
template<typename T>
Math::Rect getLocalBounds(T& text)
{
    return text.getLocalBounds();
}
template<typename T>
Math::Vector2 getOrigin(T& trans)
{
    return trans.getOrigin();
}
template<typename T>
Math::Vector2 getPosition(T& trans)
{
    return trans.getPosition();
}
template<typename T>
Math::Vector2 getScale(T& trans)
{
    return trans.getScale();
}
template<typename T>
Math::Vector2 getPoint(unsigned int uint, T& shape)
{
    return shape.getPoint(uint);
}
template<typename T>
Math::Rect getTextureRect(T& shape)
{
    return shape.getTextureRect();
}
#else
template<typename T>
void getGlobalBounds(asIScriptGeneric* gen)
{
    T& obj = *reinterpret_cast<T*>(gen->GetObject());
    new(gen->GetAddressOfReturnLocation()) Math::Rect(obj.getGlobalBounds());
}
template<typename T>
void getLocalBounds(asIScriptGeneric* gen)
{
    T& obj = *reinterpret_cast<T*>(gen->GetObject());
    new(gen->GetAddressOfReturnLocation()) Math::Rect(obj.getLocalBounds());
}
template<typename T>
void getOrigin(asIScriptGeneric* gen)
{
    T& obj = *reinterpret_cast<T*>(gen->GetObject());
    new(gen->GetAddressOfReturnLocation()) Math::Vector2(obj.getOrigin());
}
template<typename T>
void getPosition(asIScriptGeneric* gen)
{
    T& obj = *reinterpret_cast<T*>(gen->GetObject());
    new(gen->GetAddressOfReturnLocation()) Math::Vector2(obj.getPosition());
}
template<typename T>
void getScale(asIScriptGeneric* gen)
{
    T& obj = *reinterpret_cast<T*>(gen->GetObject());
    new(gen->GetAddressOfReturnLocation()) Math::Vector2(obj.getScale());
}
template<typename T>
void getPoint(asIScriptGeneric* gen)
{
    T& obj = *reinterpret_cast<T*>(gen->GetObject());
    uint32_t point = gen->GetArgDWord(0);
    new (gen->GetAddressOfReturnLocation()) Math::Vector2(obj.getPoint(point));
}
template<typename T>
void getTextureRect(asIScriptGeneric* gen)
{
    T& obj = *reinterpret_cast<T*>(gen->GetObject());
    new(gen->GetAddressOfReturnLocation()) Math::Rect(obj.getTextureRect());
}
#endif
template<typename T>
void setOrigin(const Math::Vector2& vec, T& trans)
{
    trans.setOrigin(vec);
}
template<typename T>
void setPosition(const Math::Vector2& vec, T& trans)
{
    trans.setPosition(vec);
}

template<typename T>
void setScale(const Math::Vector2& vec, T& trans)
{
    trans.setScale(vec);
}
template<typename T>
void setScaleFloat(float scale, T& trans)
{
    trans.setScale(scale, scale);
}

template<typename T>
void move(const Math::Vector2& vec, T& trans)
{
    trans.move(vec);
}
template<typename T>
void scale(const Math::Vector2& vec, T& trans)
{
    trans.scale(vec);
}

template<typename T>
void destruct_shape(T* shape)
{
    shape->~T();

    if (shapes.count(shape) == 0)
        return;

    shapes[shape]->GCRelease();
    shapes.erase(shape);
}
template<typename T>
T& assign_shape(const T& rhs, T& lhs)
{
    if (shapes.count(const_cast<T*>(&rhs)) > 0)
        shapes[const_cast<T*>(&rhs)]->GCAddRef();
    if (shapes.count(&lhs) > 0)
        shapes[&lhs]->GCRelease();

    lhs = rhs;

    if (shapes.count(const_cast<T*>(&rhs)) > 0)
        shapes[&lhs] = shapes[const_cast<T*>(&rhs)];

    return lhs;
}

template<typename T>
Texture_t* getTexture(T& shape)
{
    return shapes.at(&shape);
}
template<typename T>
void setTextureReset(Texture_t* texture, bool resetRect, T& shape)
{
    if (!texture)
        return;

    if (shapes.count(&shape))
        shapes[&shape]->GCRelease();

    shape.setTexture(*texture, resetRect);

    shapes[&shape] = texture;
}

template<typename T>
void setTextureRect(const Math::Rect& rect, T& shape)
{
    shape.setTextureRect(rect);
}

}

namespace Script
{

namespace SFML
{

template<typename T>
void registerTransformable(const char* name, asIScriptEngine* eng)
{
    int r = 0;

#ifdef AS_SUPPORT_VALRET
    r = eng->RegisterObjectMethod(name, "Vec2 get_Origin()", asFUNCTION(getOrigin<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "Vec2 get_Position()", asFUNCTION(getPosition<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "Vec2 get_Scale()", asFUNCTION(getScale<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);
#else
    r = eng->RegisterObjectMethod(name, "Vec2 get_Origin()", asFUNCTION(getOrigin<T>), asCALL_GENERIC); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "Vec2 get_Position()", asFUNCTION(getPosition<T>), asCALL_GENERIC); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "Vec2 get_Scale()", asFUNCTION(getScale<T>), asCALL_GENERIC); assert(r >= 0);
#endif
    
    r = eng->RegisterObjectMethod(name, "void set_Origin(Vec2&in)", asFUNCTION(setOrigin<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void SetOrigin(float,float)", asMETHODPR(T, setOrigin, (float, float), void), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void set_Position(Vec2&in)", asFUNCTION(setPosition<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void SetPosition(float,float)", asMETHODPR(T, setPosition, (float, float), void), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "float get_Rotation()", asMETHODPR(T, getRotation, (void) const, float), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void set_Rotation(float)", asMETHODPR(T, setRotation, (float), void), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void set_Scale(Vec2&in)", asFUNCTION(setScale<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void SetScale(float)", asFUNCTION(setScaleFloat<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void SetScale(float,float)", asMETHODPR(T, setScale, (float, float), void), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void Move(float,float)", asMETHODPR(T, move, (float, float), void), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void Move(Vec2&in)", asFUNCTION(move<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void Rotate(float)", asMETHODPR(T, rotate, (float), void), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void Scale(float,float)", asMETHODPR(T, scale, (float, float), void), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void Scale(Vec2&in)", asFUNCTION(scale<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);

    Script::ScriptManager.registerType(name, []() {
        return new CSFMLType<T>();
    });
}

template<typename T>
void registerShape(const char* name, asIScriptEngine* eng)
{
    int r = 0;

    r = eng->RegisterObjectBehaviour(name, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destruct_shape<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);

    r = eng->RegisterObjectMethod(name, (std::string(name) + "& opAssign(" + name + "&in)").c_str(), asFUNCTION(assign_shape<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);
    
#ifdef AS_SUPPORT_VALRET
    r = eng->RegisterObjectMethod(name, "Vec2 opIndex(uint) const", asFUNCTION(getPoint<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);
    
    r = eng->RegisterObjectMethod(name, "Rect get_GlobalBounds()", asFUNCTION(getGlobalBounds<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "Rect get_LocalBounds()", asFUNCTION(getGlobalBounds<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "Rect get_TextureRect()", asFUNCTION(getTextureRect<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);
#else
    r = eng->RegisterObjectMethod(name, "Vec2 opIndex(uint) const", asFUNCTION(getPoint<T>), asCALL_GENERIC); assert(r >= 0);

    r = eng->RegisterObjectMethod(name, "Rect get_GlobalBounds()", asFUNCTION(getGlobalBounds<T>), asCALL_GENERIC); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "Rect get_LocalBounds()", asFUNCTION(getGlobalBounds<T>), asCALL_GENERIC); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "Rect get_TextureRect()", asFUNCTION(getTextureRect<T>), asCALL_GENERIC); assert(r >= 0);
#endif

    

    r = eng->RegisterObjectMethod(name, "Color& get_FillColor()", asMETHOD(T, getFillColor), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void set_FillColor(Color&in)", asMETHODPR(T, setFillColor, (const sf::Color&), void), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "Color& get_OutlineColor()", asMETHOD(T, getOutlineColor), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void set_OutlineColor(Color&in)", asMETHODPR(T, setOutlineColor, (const sf::Color&), void), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "float get_OutlineThickness()", asMETHODPR(T, getOutlineThickness, () const, float), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void set_OutlineThickness(float)", asMETHODPR(T, setOutlineThickness, (float), void), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "uint get_PointCount()", asMETHODPR(T, getPointCount, () const, unsigned int), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "Texture@ GetTexture()", asFUNCTION(getTexture<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void SetTexture(Texture@,bool resetTextureRect=false)", asFUNCTION(setTextureReset<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void set_TextureRect(Rect&in)", asFUNCTION(setTextureRect<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);

    r = eng->RegisterObjectMethod("::Renderer", ("void Draw(Shapes::" + std::string(name) + "&in)").c_str(), asFUNCTION(drawShape<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = eng->RegisterObjectMethod("::Renderer", ("void Draw(Shapes::" + std::string(name) + "&in,Shader@)").c_str(), asFUNCTION(drawShape_shader<T>), asCALL_CDECL_OBJLAST); assert(r >= 0);

    registerTransformable<T>(name, eng);
}

template<typename T>
void registerSoundSource(const char* name, asIScriptEngine* eng)
{
    int r = 0;

    r = eng->RegisterObjectMethod(name, "float get_Pitch()", asMETHOD(T, getPitch), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void set_Pitch(float)", asMETHOD(T, setPitch), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "float get_Volume()", asMETHOD(T, getVolume), asCALL_THISCALL); assert(r >= 0);
    r = eng->RegisterObjectMethod(name, "void set_Volume(float)", asMETHOD(T, setVolume), asCALL_THISCALL); assert(r >= 0);
}

}

}