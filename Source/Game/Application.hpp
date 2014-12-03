#pragma once

#include <View/Camera.hpp>

#include <SFML/Graphics/RenderWindow.hpp>

class Application
{
public:
    Application();
    Application(const Application&) = delete;
    ~Application() = default;

    Application& operator=(const Application&) = delete;

    void runGameLoop();


    static const uint8_t TICKRATE = 33;

private:
    sf::RenderWindow mWindow;

    View::Camera mGameCamera, mUICamera;
};
