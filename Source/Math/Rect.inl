#pragma once

template<typename T>
Math::Rect::Rect(const sf::Rect<T>& rect) : 
    Top((float)rect.top), Left((float)rect.left), Width((float)rect.width), Height((float)rect.height)
{

}

template<typename T>
Math::Rect::operator sf::Rect<T>() const
{
    return sf::Rect<T>((T)Top, (T)Left, (T)Width, (T)Height);
}
