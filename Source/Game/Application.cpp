#include "Application.hpp"
#include "Resources.hpp"

#include <Input/InputManager.hpp>
#include <Script/ScriptHooks.hpp>
#include <Script/ScriptManager.hpp>
#include <Script/ScriptObject.hpp>
#include <Util/FileSystem.hpp>

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Window/Event.hpp>

namespace
{
    namespace Tick
    {
        const Util::Timespan OneTick(std::chrono::nanoseconds(std::nano::den) / (int)Application::TICKRATE);
        const float OneTickFloat = std::chrono::duration<float>(Tick::OneTick).count();
    }
    const Util::Timespan MaxFrameTime(500 * (std::nano::den / std::milli::den));

    enum Binds
    {
        Bind_Up,
        Bind_Left,

        Bind_Action,

        Bind_Down,
        Bind_Right,

        Bind_Count
    };

    sf::RenderWindow* sRW;

    Math::Vector2 getMouse(sf::RenderTarget* target)
    {
        return sf::Mouse::getPosition(*sRW);
    }
}

Application::Application(asIScriptEngine* eng) : mEngine(eng)
{
    Input::InputManager.setBindCount(Bind_Count);
    Input::InputManager.linkInputs(Bind_Up, Bind_Down);
    Input::InputManager.linkInputs(Bind_Left, Bind_Right);


    Input::InputManager.bindInput(Bind_Up, Input::Input::Bind{ Input::Input::Bind::Bind_Keyboard, { { sf::Keyboard::W, (uint8_t)0 } } });
    Input::InputManager.bindInput(Bind_Left, Input::Input::Bind{ Input::Input::Bind::Bind_Keyboard, { { sf::Keyboard::A, (uint8_t)0 } } });
    Input::InputManager.bindInput(Bind_Action, Input::Input::Bind{ Input::Input::Bind::Bind_Keyboard, { { sf::Keyboard::Space, (uint8_t)0 } } });


    Script::ScriptHooks::addHook("Draw", "void f(Renderer@)");
    Script::ScriptHooks::addHook("DrawUi", "void f(Renderer@)");

    Script::ScriptHooks::addHook("Tick", "void f(float)");
    Script::ScriptHooks::addHook("Update", "void f(float)");

    Script::ScriptHooks::addHook("Packet", "void f(Packet&in)");

    mEngine->RegisterGlobalFunction("void QuitGame()", asMETHOD(Application, quitGame), asCALL_THISCALL_ASGLOBAL, this);

    mEngine->SetDefaultNamespace("Network");
    mEngine->RegisterGlobalFunction("bool SendPacket(Packet&in)", asMETHOD(Application, sendPacket), asCALL_THISCALL_ASGLOBAL, this);
    mEngine->RegisterGlobalFunction("void Disconnect()", asMETHOD(Application, disconnect), asCALL_THISCALL_ASGLOBAL, this);
    mEngine->RegisterGlobalFunction("bool Connect()", asMETHOD(Application, connect), asCALL_THISCALL_ASGLOBAL, this);
    mEngine->RegisterGlobalFunction("bool get_Connected()", asMETHOD(Application, connected), asCALL_THISCALL_ASGLOBAL, this);
    mEngine->SetDefaultNamespace("");

    mEngine->RegisterObjectMethod("Renderer", "Vec2 get_MousePos()", asFUNCTION(getMouse), asCALL_CDECL_OBJLAST);

    Resources::Shaders.add("Well", "well.frag", sf::Shader::Fragment);

    if (Util::FileSystem::isFolder("Scripts"))
    {
        for (auto& script : Util::FileSystem::findFiles(Util::FileSystem::fixPath("Scripts/*.as")))
        {
            Script::ScriptManager.loadScriptFromFile(script);
        }
    }
}

void Application::runGameLoop()
{
    Script::ScriptObject* menu = Script::ScriptObject::Create("StateMachine");

    mWindow.create(sf::VideoMode(800, 600), "LD31");
    sRW = &mWindow;

    mGameCamera.setView(mWindow.getDefaultView());
    mGameCamera.setKeepRatio();
    mUICamera.setView(mWindow.getDefaultView());
    
    Util::Timestamp oldFrame = Util::ClockImpl::now();
    Util::Timespan totalTime = Util::Timespan(0);

    sf::Event ev;
    while (mWindow.isOpen())
    {
        Util::Timestamp now = Util::ClockImpl::now();
        Util::Timespan frameTime = std::min(now - oldFrame, MaxFrameTime);
        totalTime += frameTime;

        while (mWindow.pollEvent(ev))
        {
            switch (ev.type)
            {
            case sf::Event::Closed:
                mWindow.close();
                break;

            case sf::Event::LostFocus:
                Input::InputManager.disable();
                break;

            case sf::Event::GainedFocus:
                Input::InputManager.disable(false);
                break;

            case sf::Event::Resized:
            {
                Math::Vector2 size((float)ev.size.width, (float)ev.size.height);

                mGameCamera.notifyDisplayUpdate(size);
                mUICamera.notifyDisplayUpdate(size);
                mUICamera.setCenter(size / 2);
            } break;

            default:
                Input::InputManager.handleEvent(ev);
                break;
            }
        }

        sf::Packet p;
        while (totalTime > Tick::OneTick)
        {
            Script::ScriptManager.checkForModification();
            auto status = mSocket.receive(p);
            if (status == sf::Socket::Done)
                Script::ScriptHooks::execute<const sf::Packet&>("Packet", mEngine, p);

            Input::InputManager.update();
            Script::ScriptManager.runCoroutines(Tick::OneTick / 2);

            // TODO: Run game ticks here
            Script::ScriptHooks::execute<float>("Tick", mEngine, Tick::OneTickFloat);

            totalTime -= Tick::OneTick;
        }
        
        // TODO: Run game variadic update here

        Script::ScriptHooks::execute<float>("Update", mEngine, std::chrono::duration<float>(frameTime).count());


        mWindow.clear();

        mWindow.setView(mGameCamera.getView());

        // TODO: Draw Game

        Script::ScriptHooks::execute<sf::RenderTarget*>("Draw", mEngine, &mWindow);

        mWindow.setView(mUICamera.getView());

        // TODO: Draw UI

        Script::ScriptHooks::execute<sf::RenderTarget*>("DrawUi", mEngine, &mWindow);

        mWindow.display();

        oldFrame = now;

        Util::sleep(Util::Timespan(std::chrono::milliseconds(5)));
    }

    delete menu;
}

void Application::quitGame()
{
    mWindow.close();
}

bool Application::connected()
{
    return mSocket.getRemoteAddress() != sf::IpAddress::None;
}

void Application::disconnect()
{
//    sf::Packet disc;
//    disc << (uint8_t)0 << (uint16_t)0xDEAD;
//    mSocket.send(disc);

    mSocket.disconnect();
}

bool Application::connect()
{
    if (connected())
        return true;

    auto status = mSocket.connect(sf::IpAddress(130, 236, 254, 211), 23971, sf::milliseconds(1500));
    if (status == sf::Socket::Done)
        mSocket.setBlocking(false);

    return status == sf::Socket::Done;
}

bool Application::sendPacket(const sf::Packet& packet)
{
    sf::Packet copy = packet;
    return mSocket.send(copy) == sf::Socket::Done;
}

sf::RenderWindow* Application::GetRW()
{
    return sRW;
}
