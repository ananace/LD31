#pragma once

#include "Vector.hpp"

#include <SFML/Graphics/Rect.hpp>

namespace Math
{

struct Rect
{
    Rect();
    Rect(float top, float left, float width, float height);
    Rect(const Vector2& topleft, float width, float height);
    Rect(const Vector2& topleft, const Vector2& bottomright);
    
    template<typename T>
    Rect(const sf::Rect<T>& rect);

    template<typename T>
    operator sf::Rect<T>();

    bool operator==(const Rect& rhs) const;

    Rect& operator=(Rect other);

    Vector2 getTopLeft() const;
    Vector2 getBottomRight() const;

    Vector2 constrain(const Vector2& point);

    bool contains(const Vector2& point);
    bool intersects(const Rect& rect);
    bool intersects(const Rect& rect, Rect& intersecting);

    float Top, Left, Width, Height;
};

}

#include "Rect.inl"
