#pragma once

#include <View/Camera.hpp>

#include <SFML/Network/TcpSocket.hpp>
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

    bool connect();
    bool connected();

    bool sendPacket(const sf::Packet&);

    static const uint8_t TICKRATE = 33;

private:
    asIScriptEngine* mEngine;
    sf::RenderWindow mWindow;
    sf::TcpSocket mSocket;

    View::Camera mGameCamera, mUICamera;
};
