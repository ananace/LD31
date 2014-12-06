#pragma once

#include <View/Camera.hpp>

#include <SFML/Graphics/RenderWindow.hpp>

class asIScriptEngine;

class Application
{
public:
    Application(asIScriptEngine* eng);
    Application(const Application&) = delete;
    ~Application() = default;

    Application& operator=(const Application&) = delete;

    void runGameLoop();
    void quitGame();

    static const uint8_t TICKRATE = 33;

private:
    asIScriptEngine* mEngine;
    sf::RenderWindow mWindow;

    View::Camera mGameCamera, mUICamera;
};
