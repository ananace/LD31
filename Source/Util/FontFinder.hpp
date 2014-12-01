#pragma once

#include <SFML/Graphics/Font.hpp>

namespace Util
{

class FontFinder
{
public:
    static sf::Font& getDefaultFont();
    static bool findFont(sf::Font& out, const std::string& name, const std::string& style = "Regular");

private:
    static sf::Font* sCachedDefault;
};

}