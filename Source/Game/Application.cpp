#include "Application.hpp"

#include <Input/InputManager.hpp>
#include <Script/ScriptManager.hpp>

#include <SFML/Window/Event.hpp>

namespace
{
    namespace Tick
    {
        const Util::Timespan OneTick(std::chrono::nanoseconds(std::nano::den) / (int)Application::TICKRATE);
    }
    const Util::Timespan MaxFrameTime(500 * (std::nano::den / std::milli::den));
}

Application::Application()
{

}

void Application::runGameLoop()
{
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
            } break;
            }
        }

        while (totalTime > Tick::OneTick)
        {
            Input::InputManager.update();
            Script::ScriptManager.runCoroutines(Tick::OneTick / 2);

            // TODO: Run game ticks here

            totalTime -= Tick::OneTick;
        }
        
        // TODO: Run game variadic update here

        mWindow.clear();

        mWindow.setView(mGameCamera.getView());

        // TODO: Draw Game

        mWindow.setView(mUICamera.getView());

        // TODO: Draw UI

        mWindow.display();

        oldFrame = now;
    }
}
