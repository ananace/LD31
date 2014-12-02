#include <type_traits>

template<typename T>
void Input::Manager::setCurve(sf::Joystick::Axis axis)
{
    static_assert(std::is_base_of<JoystickCurve, T>::value, "T must be a proper Joystick Curve");

    JoystickCurve* oldCurve = mCurvesPerAxis[axis];
    JoystickCurve* newCurve = new T();

    newCurve->Deadzone = oldCurve->Deadzone;

    mCurvesPerAxis[axis] = newCurve;
    delete oldCurve;
}
