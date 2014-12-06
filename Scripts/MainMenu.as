#include "IState.as"

namespace States
{

class MainMenu : IState
{
	MainMenu()
	{

	}

	void Update(float dt)
	{

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

		menuEntry.String = "New Game";
		menuEntry.Origin = menuEntry.LocalBounds.Size / 2;

		rend.Draw(menuEntry);
	}

	string Name
	{
		get const { return "Main Menu"; }
	}
}

}
