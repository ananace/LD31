#include "Games/IGame.as"
#include "IState.as"

namespace States
{

class GameState : IState
{
	void Update(float dt)
	{
		if (mFocusedGame is null)
		{
			for (uint x = 0; x < mMiniGames.width(); ++x)
				for (uint y = 0; y < mMiniGames.height(); ++y)
					mMiniGames[x, y].Update(dt);
		}
		else
			mFocusedGame.Update(dt);
	}

	void Tick(float dt)
	{
		if (mFocusedGame !is null)
			mFocusedGame.Tick(dt);
	}

	void Draw(Renderer@ rend)
	{
		if (mFocusedGame !is null)
			mFocusedGame.DrawFull(rend);
		else
		{
			Rect gameRect(0, 0, 256, 256);

			for (uint x = 0; x < mMiniGames.width(); ++x)
				for (uint y = 0; y < mMiniGames.height(); ++y)
				{
					gameRect.TopLeft = Vec2(x * 256, y * 256);

					mMiniGames[x, y].DrawQuick(rend, gameRect);
				}
		}	
	}

	void DrawUi(Renderer@ rend)
	{
		
	}

	string Name
	{
		get const { return "Game State"; }
	}

	private grid<Games::IGame@> mMiniGames;
	private Games::IGame@ mFocusedGame;
}

}
