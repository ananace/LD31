#include "Rect.hpp"
#include "Common.hpp"

using namespace Math;

Rect::Rect() :
    Top(0), Left(0), Width(0), Height(0)
{
}
Rect::Rect(float top, float left, float width, float height)
{
    if (width < 0)
    {
        left = left - width;
        width = -width;
    }
    if (height < 0)
    {
        top = top - height;
        height = -height;
    }

    Top = top;
    Left = left;
    Width = width;
    Height = height;
}
Rect::Rect(const Vector2& topleft, float width, float height)
{
    float top = topleft.Y, left = topleft.X;

    if (width < 0)
    {
        left = left - width;
        width = -width;
    }
    if (height < 0)
    {
        top = top - height;
        height = -height;
    }

    Top = top;
    Left = left;
    Width = width;
    Height = height;
}
Rect::Rect(const Vector2& topleft, const Vector2& bottomright) :
    Top(topleft.Y), Left(topleft.X), Width(bottomright.X - topleft.X), Height(bottomright.Y - topleft.Y)
{
}

bool Rect::operator==(const Rect& rhs) const
{
    return FloatCompare(Top, rhs.Top, 0.0000001f) && FloatCompare(Left, rhs.Left, 0.0000001f) && FloatCompare(Width, rhs.Width, 0.0000001f) && FloatCompare(Height, rhs.Height, 0.0000001f);
}

Rect& Rect::operator=(Rect other)
{
    std::swap(Top, other.Top);
    std::swap(Left, other.Left);
    std::swap(Width, other.Width);
    std::swap(Height, other.Height);

    return *this;
}

Vector2 Rect::getTopLeft() const
{
    return Vector2(Left, Top);
}
Vector2 Rect::getBottomRight() const
{
    return Vector2(Left + Width, Top + Height);
}

Vector2 Rect::constrain(const Vector2& point)
{
    return Vector2(point.X < Left ? Left : (point.X > Left + Width ? Left + Width : point.X), point.Y < Top ? Top : (point.Y > Top + Height ? Top + Height : point.Y));
}

bool Rect::contains(const Vector2& point)
{
    return Left <= point.X && Top <= point.Y && Left + Width >= point.X && Top + Height >= point.Y;
}
bool Rect::intersects(const Rect& rect)
{
    float interLeft = std::max(Left, rect.Left),
        interTop = std::max(Top, rect.Top),
        interRight = std::max(Left + Width, rect.Left + rect.Width),
        interBottom = std::max(Top + Height, rect.Top + rect.Height);

    return (interLeft < interRight && interTop < interBottom);
}
bool Rect::intersects(const Rect& rect, Rect& intersecting)
{
    float interLeft = std::max(Left, rect.Left),
        interTop = std::max(Top, rect.Top),
        interRight = std::max(Left + Width, rect.Left + rect.Width),
        interBottom = std::max(Top + Height, rect.Top + rect.Height);

    if (interLeft < interRight && interTop < interBottom)
    {
        intersecting = Rect(interTop, interLeft, interRight - interLeft, interBottom - interTop);
        return true;
    }

    intersecting = Rect();
    return false;
}