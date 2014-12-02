#include "Input.hpp"

const float Input::Input::PRESS_PERCENTAGE = 0.75f;

Input::Input::Input() :
    mValue(0), mBind({Bind::Bind_None})
{

}
Input::Input::~Input()
{

}

Input::Input::Bind Input::Input::getBind() const
{
    return mBind;
}
float Input::Input::getValue() const
{
    return mValue;
}
bool Input::Input::isPressed() const
{
    return mValue >= PRESS_PERCENTAGE;
}
