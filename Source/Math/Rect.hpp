#pragma once

#include "Vector.hpp"

#include <SFML/Graphics/Rect.hpp>

namespace Math
{

/** \brief A class defining an area as a 2D axis-aligned rectangle
 *
 * 
 */
struct Rect
{
    /// Zeroing construct
    Rect();
    /// Value constructor
    Rect(float left, float top, float width, float height);
    /// Value constructor, using a top left position and a width + height
    Rect(const Vector2& topleft, float width, float height);
    /// Constructor from two corners
    Rect(const Vector2& topleft, const Vector2& bottomright);
    /// Default copy constructor
    Rect(const Rect&) = default;
    /// Default destructor
    ~Rect() = default;
    
    /// Constructor from SFML rect values
    template<typename T>
    Rect(const sf::Rect<T>& rect);
    /// Implicit casting to SFML rect values
    template<typename T>
    operator sf::Rect<T>() const;

    ///\brief Comparison operator
    ///\note Uses an epsilon value of `1 / 1000000` for the comparsion
    bool operator==(const Rect& rhs) const;

    /// Default assignment operator
    Rect& operator=(const Rect& other) = default;

    /// Gets the top left corner
    Vector2 getTopLeft() const;
    /// Gets the bottom right corner
    Vector2 getBottomRight() const;
    /// Gets the center of the rect
    Vector2 getCenter() const;
    /// Gets the width and height as a 2D vector
    Vector2 getSize() const;

    /// Constrains a point to the rectangle
    Vector2 constrain(const Vector2& point);

    /// Checks if the rectangle contains a point
    bool contains(const Vector2& point);
    /// Checks if the rectangle intersects with another one
    bool intersects(const Rect& rect);
    /// Checks for intersection and stores the union between this and the other rect
    bool intersects(const Rect& rect, Rect& intersecting);

    float Top, ///< Top position
          Left, ///< Left position
          Width, ///< Width of the rectangle
          Height; ///< Height of the rectangle
};

}

#include "Rect.inl"
