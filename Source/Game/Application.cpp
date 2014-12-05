#include "Application.hpp"

#include <Input/InputManager.hpp>
#include <Script/ScriptHooks.hpp>
#include <Script/ScriptManager.hpp>
#include <Script/ScriptObject.hpp>
#include <Util/FileSystem.hpp>

#include <SFML/Window/Event.hpp>

namespace
{
    namespace Tick
    {
        const Util::Timespan OneTick(std::chrono::nanoseconds(std::nano::den) / (int)Application::TICKRATE);
        const float OneTickFloat = std::chrono::duration<float>(Tick::OneTick).count();
    }
    const Util::Timespan MaxFrameTime(500 * (std::nano::den / std::milli::den));
}

Application::Application(asIScriptEngine* eng) : mEngine(eng)
{
    Script::ScriptHooks::addHook("Draw", "void f(Renderer@)");
    Script::ScriptHooks::addHook("DrawUi", "void f(Renderer@)");

    Script::ScriptHooks::addHook("Tick", "void f(float)");
    Script::ScriptHooks::addHook("Update", "void f(float)");

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
            }
        }

        while (totalTime > Tick::OneTick)
        {
            Script::ScriptManager.checkForModification();

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
