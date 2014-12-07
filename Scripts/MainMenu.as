#include "IState.as"
#include "GameState.as"
#include "NetworkUI.as"
#include "Options.as"

namespace States
{

class MainMenu : IState
{
	MainMenu()
	{

	}

	void Packet(Packet&in){}
	void Init(StateMachine@ stateman)
	{
		@mStateMan = @stateman;
	}

	void Update(float dt)
	{
		mWasPressed = mIsPressed;
		mIsPressed = Mouse::IsPressed(Mouse::Button::Left) && !Inputs::Disabled;
	}

	void Tick(float)
	{

	}

	void Draw(Renderer@)
	{

	}

	void DrawUi(Renderer@ rend)
	{
		Text menuEntry;

		const string[] entries = {
			"Hotseat Game",
			"Internet Game",
			"Options",
			"End Game"
		};
		const float entryHeight = 42;

		menuEntry.Position = Vec2(8, rend.View.Size.Y / 2);

		for (uint i = 0; i < entries.length; ++i)
		{
			bool pressed = false;
			menuEntry.String = entries[i];

			if (i == 0)
			{
				menuEntry.Move(0, entryHeight * -entries.length / 2);
			}

			menuEntry.Origin = Vec2(0, menuEntry.LocalBounds.Size.Y / 2);

			if (menuEntry.GlobalBounds.Contains(rend.MousePos))
			{
				menuEntry.Color = Colors::Yellow;

				if (mIsPressed)
				{
					Color temp;
					temp.R = int(255 * 0.75);
					temp.G = int(255 * 0.75);

					menuEntry.Color = temp;
					menuEntry.Move(2, 2);
					pressed = true;
				}
				else if (mWasPressed)
				{
					string name = entries[i];
					if (name == "Hotseat Game")
					{
						States::GameState@ game = GameState();
						mStateMan.PushState(game);
					}
					else if (name == "Internet Game")
					{
						if (!Network::Connected && !Network::Connect())
							println("Can't connect right now, check your interwebs");
						else
							mStateMan.PushState(States::NetworkUI());
					}
					else if (name == "Options")
					{
						States::Options@ options = Options();
						mStateMan.PushState(options);
					}
					else if (name == "End Game")
					{
						QuitGame();
					}
				}
			}
			else
				menuEntry.Color = Colors::White;

			rend.Draw(menuEntry);

			if (pressed)
				menuEntry.Move(-2, -2);

			menuEntry.Move(0, entryHeight);
		}
	}

	string Name
	{
		get const { return "Main Menu"; }
	}

	private bool mIsPressed, mWasPressed;
	private StateMachine@ mStateMan;
}

}
