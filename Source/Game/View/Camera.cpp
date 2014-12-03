#include "Camera.hpp"
#include <Math/Common.hpp>
#include <Math/Rect.hpp>

#include <cmath>

using View::Camera;

namespace
{
    float clamp(float min, float val, float max)
    {
        return fmin(fmax(min, val), max);
    }
}

Camera::Camera() : 
    mMaxZoom(2.f), mMinZoom(0.5f), mKeepRatio(false), mAutomatic(false)
{

}

float Camera::getMaxZoom() const
{
    return mMaxZoom;
}
void Camera::setMaxZoom(float max)
{
    mMaxZoom = max;
}
float Camera::getMinZoom() const
{
    return mMinZoom;
}
void Camera::setMinZoom(float min)
{
    mMinZoom = min;
}

bool Camera::isAutomatic() const
{
    return mAutomatic;
}
void Camera::setAutomatic(bool automatic)
{
    mAutomatic = automatic;

    if (!mAutomatic)
    {
        mPOI.clear();
        mVIP.clear();
    }
}

bool Camera::isRatioKept() const
{
    return mKeepRatio;
}
void Camera::setKeepRatio(bool keep)
{
    mKeepRatio = keep;
}

float Camera::getRotation() const
{
    return mView.getRotation() * Math::Deg2Rad;
}
void Camera::setRotation(float rotation)
{
    mView.setRotation(rotation * Math::Rad2Deg);
}
void Camera::rotate(float delta)
{
    mView.rotate(delta * Math::Rad2Deg);
}

Math::Vector2 Camera::getCenter() const
{
    return mView.getCenter();
}
void Camera::setCenter(const Math::Vector2& center)
{
    mView.setCenter(center);
}
void Camera::move(const Math::Vector2& delta)
{
    mView.move(delta);
}

float Camera::getZoom() const // mView.getSize() / mBaseSize
{
    return (mBaseSize / mView.getSize()).getLength();
}
void Camera::setZoom(float zoom) // mView.setSize(mBaseSize * zoom)
{
    zoom = clamp(mMinZoom, zoom, mMaxZoom);

    mView.setSize(mBaseSize / zoom);
}
void Camera::zoom(float delta) // mView.zoom(delta)
{
    float curZoom = getZoom();
    float targetZoom = clamp(mMinZoom, curZoom + delta, mMaxZoom);
    setZoom(targetZoom);
}

void Camera::addPoI(const Math::Vector2& point, const Util::Timespan& duration, bool vip)
{
    if (!mAutomatic)
        return;

    if (vip)
        mVIP.push_back(std::make_pair(point, Util::ClockImpl::now() + duration));
    else
        mPOI.push_back(std::make_pair(point, Util::ClockImpl::now() + duration));
}

void Camera::update(const Util::Timespan& dt)
{
    if (!mAutomatic)
        return;

    auto now = Util::ClockImpl::now();

}

void Camera::notifyDisplayUpdate(const Math::Vector2& size)
{
    Math::Rect viewPort = mView.getViewport();
    Math::Vector2 realNewSize = size * viewPort.getSize();

    if (!mKeepRatio)
    {
        float zoom = getZoom();
        mBaseSize = realNewSize;
        setZoom(zoom);
    }
    else
    {
        float zoom = getZoom();

        float aspect = realNewSize.X / realNewSize.Y;
        mBaseSize.X = mBaseSize.Y * aspect;

        setZoom(zoom);
    }
}

void Camera::setView(const sf::View& view)
{
    mView = view;
    mBaseSize = mView.getSize();
}
sf::View Camera::getView() const
{
    return mView;
}