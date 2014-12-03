#pragma once

#include <SFML/Graphics/Font.hpp>

namespace Util
{

/// A helper class for finding fonts in the system
class FontFinder
{
public:
    /// Gets the default font for the system
    static sf::Font& getDefaultFont();
    /// Tries to find a font by name and style
    static bool findFont(sf::Font& out, const std::string& name, const std::string& style = "Regular");

private:
    /// The cached default font
    static sf::Font* sCachedDefault;
};

}