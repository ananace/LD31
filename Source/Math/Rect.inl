#pragma once

using namespace Math;

template<typename T>
Rect::Rect(const sf::Rect<T>& rect) : 
    Top(rect.top), Left(rect.left), Width(rect.width), Height(rect.height)
{

}

template<typename T>
Rect::operator sf::Rect<T>()
{
    return sf::Rect<T>(Top, Left, Width, Height);
}
