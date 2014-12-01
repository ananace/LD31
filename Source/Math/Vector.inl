#pragma once

template<typename T>
Math::Vector2::Vector2(const sf::Vector2<T>& vec) :
    X((float)vec.x), Y((float)vec.y)
{

}

template<typename T>
Math::Vector2::operator sf::Vector2<T>() const
{
    return sf::Vector2<T>((T)X, (T)Y);
}
