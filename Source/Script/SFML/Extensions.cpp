#include "Extensions.hpp"
#include "Extensions.inl"

#include <Util/ResourceManager.hpp>
#include <SFML/Graphics/Texture.hpp>

std::unordered_map<void*, Texture_t*> Script::SFML::shapes;

bool Script::SFML::RegisteredExtensions()
{
    const int TotalExtensions = 13;

    int ret = 0;
    ret += Extensions::CircleShape;
    ret += Extensions::Color;
    ret += Extensions::ConvexShape;
    ret += Extensions::Font;
    ret += Extensions::Music;
    ret += Extensions::RectangleShape;
    ret += Extensions::RenderTarget;
    ret += Extensions::Shader;
    ret += Extensions::Sound;
    ret += Extensions::Sprite;
    ret += Extensions::Text;
    ret += Extensions::Texture;
    ret += Extensions::View;

    return ret == TotalExtensions;
}