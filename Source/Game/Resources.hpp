#pragma once

#include <Util/ResourceManager.hpp>

#include <SFML/Audio/Music.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace Resources
{
    extern Util::ResourceManager<sf::Music, std::string> Musics;
    extern Util::ResourceManager<sf::Shader, std::string> Shaders;
    extern Util::ResourceManager<sf::SoundBuffer, std::string> Sounds;
    extern Util::ResourceManager<sf::Texture, std::string> Textures;
}
