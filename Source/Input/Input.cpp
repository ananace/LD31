#include "Input.hpp"

const float Input::Input::PRESS_PERCENTAGE = 0.75f;

Input::Input::Input() :
    mLinkedInput(nullptr), mValue(0), mBind({ Bind::Bind_None })
{
}

Input::Input::Bind Input::Input::getBind() const
{
    return mBind;
}
const Input::Input& Input::Input::getLinked() const
{
    return *mLinkedInput;
}
float Input::Input::getValue() const
{
    return mValue;
}
float Input::Input::getCombinedValue() const
{
    if (mLinkedInput == nullptr)
        return mValue;

    return mLinkedInput->mValue - mValue;
}
int Input::Input::getCombinedPress() const
{
    if (!mLinkedInput)
        return mValue >= PRESS_PERCENTAGE;

    return (mLinkedInput->mValue >= PRESS_PERCENTAGE) - (mValue >= PRESS_PERCENTAGE);
}

bool Input::Input::isLinked() const
{
    return mLinkedInput != nullptr;
}
bool Input::Input::isPressed() const
{
    return mValue >= PRESS_PERCENTAGE;
}
