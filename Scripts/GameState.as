#include "Games/IGame.as"
#include "Games/Asteroids.as"

#include "IState.as"

namespace States
{

class GameState : IState
{
	GameState()
	{
		mMiniGames.resize(2,2);

		@mMiniGames[0, 0] = Games::Asteroids();
		@mMiniGames[1, 0] = Games::Asteroids();
		@mMiniGames[0, 1] = Games::Asteroids();
		@mMiniGames[1, 1] = Games::Asteroids();
	}

	void Init(StateMachine@)
	{

	}

	void Update(float dt)
	{
		if (mFocusedGame is null)
		{
			for (uint x = 0; x < mMiniGames.width(); ++x)
				for (uint y = 0; y < mMiniGames.height(); ++y)
				{
					Games::IGame@ game = mMiniGames[x, y];
					if (game !is null)
						game.Update(dt);
				}
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
		rend.View.Rotation = -45;
		rend.View.Center = Vec2(256, 256);
		rend.View.Size = rend.View.Size * 2;

		if (mFocusedGame !is null)
			mFocusedGame.DrawFull(rend);
		else
		{
			Rect gameRect(0, 0, 256, 256);
			const float margin = 25;

			for (uint x = 0; x < mMiniGames.width(); ++x)
				for (uint y = 0; y < mMiniGames.height(); ++y)
				{
					Games::IGame@ game = mMiniGames[x, y];
					if (game is null)
						continue;

					gameRect.Left = x * (gameRect.Width + margin);
					gameRect.Top = y * (gameRect.Height + margin);

					bool hover = gameRect.Contains(rend.PixelToCoords(rend.MousePos));
					if (hover)
					{
						gameRect.Left -= 10;
						gameRect.Top -= 10;
						gameRect.Width += 20;
						gameRect.Height += 20;

						if (Mouse::IsPressed(Mouse::Button::Left))
						{
							@mFocusedGame = @game;
						}
					}

					//println("[" + x + ", " + y + "] = " + gameRect.Left + ", " + gameRect.Top);

					game.DrawQuick(rend, gameRect);

					if (hover)
					{
						gameRect.Width -= 20;
						gameRect.Height -= 20;
					}
				}
		}	
	}

	void DrawUi(Renderer@ rend)
	{
		Text choose("Choose your next game:");

		choose.Origin = Vec2(choose.LocalBounds.Size.X / 2, 0);
		choose.Position = Vec2(rend.View.Size.X / 2, 15);

		rend.Draw(choose);
	}

	string Name
	{
		get const { return "Game"; }
	}

	private grid<Games::IGame@> mMiniGames;
	private Games::IGame@ mFocusedGame;
}

}
