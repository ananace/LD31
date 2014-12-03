#pragma once

#include <Math/Vector.hpp>
#include <Util/Time.hpp>

#include <SFML/Graphics/View.hpp>

#include <vector>

namespace View
{
    
class Camera
{
public:
    Camera();
    Camera(const Camera&) = default;
    ~Camera() = default;

    Camera& operator=(const Camera&) = default;

    float getMaxZoom() const;
    void setMaxZoom(float max);
    float getMinZoom() const;
    void setMinZoom(float min);

    bool isAutomatic() const;
    void setAutomatic(bool automatic = true);

    bool isRatioKept() const;
    void setKeepRatio(bool keep = true);

    ///\name Manual controls
    ///@{
    float getRotation() const;
    void setRotation(float rotation);
    void rotate(float delta);

    Math::Vector2 getCenter() const;
    void setCenter(const Math::Vector2& center);
    void move(const Math::Vector2& delta);

    float getZoom() const; // mView.getSize() / mBaseSize
    void setZoom(float zoom); // mView.setSize(mBaseSize * zoom)
    void zoom(float delta); // mView.zoom(delta)
    ///@}

    /** \brief Add a Point of Interest to the camera
     *
     * The camera will try to keep as many of the points in view as it can
     * \param point The point to add
     * \param duration The duration the point shall stay in the camera,
     * if the point moves then add it every frame with a duration of 0.
     * \param vip Very Important Point, forces the camera to keep this point in view
     */
    void addPoI(const Math::Vector2& point, const Util::Timespan& duration = Util::Timespan(0), bool vip = false);

    /// Eases the camera towards the best center and zoom that shows as many Points of Interest as possible
    void update(const Util::Timespan& dt);

    /// The camera display was resized, passes along the new window size
    void notifyDisplayUpdate(const Math::Vector2& size);

    void setView(const sf::View& view);
    sf::View getView() const;

private:
    float mMaxZoom, ///< Maximal zoom ratio
          mMinZoom; ///< Minimal zoom ratio
    /** \brief Should the ratio be kept
     *
     * If the ratio isn't kept, then any resizes will change the ratio of the camera. Good for UI.
     *
     * Keeping the ratio on the other hand means that fixed-size graphics won't stretch on resizing. Good for game cameras.
     */
    bool mKeepRatio,
         mAutomatic; ///< Is the camera under automatic control.

    Math::Vector2 mBaseSize; ///< The base size of the view, before zooming/scaling.
    sf::View mView; ///< The internal view object

    std::vector<std::pair<Math::Vector2, Util::Timestamp>> mPOI; ///< A list of Points of Interest
    std::vector<std::pair<Math::Vector2, Util::Timestamp>> mVIP; ///< A list of Very Important Points (of interest)
};

}
