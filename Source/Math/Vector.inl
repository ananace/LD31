#pragma once

using namespace Math;

template<typename T>
Vector2::Vector2(const sf::Vector2<T>& vec) :
    X(vec.x), Y(vec.y)
{

}

template<typename T>
Vector2::operator sf::Vector2<T>()
{
    return sf::Vector2<T>(vec.x, vec.y);
}
