#include "Games/IGame.as"
#include "Games/Asteroids.as"

#include "IState.as"

float min(float a, float b) { return (a < b ? a : b); }
float max(float a, float b) { return (a > b ? a : b); }

namespace States
{

class GameState : IState
{
	GameState()
	{
		mMiniGames.resize(4, 4);

		for (uint x = 0; x < mMiniGames.width(); ++x)
			for (uint y = 0; y < mMiniGames.height(); ++y)
				@mMiniGames[x, y] = Games::Asteroids();
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
		{
			if (mFocusedGame.Finished)
				mLerpPoint = max(mLerpPoint - dt * 2, 0);
			else
				mLerpPoint = min(mLerpPoint + dt * 2, 1);

			mFocusedGame.Update(dt);
		}
	}

	void Tick(float dt)
	{
		if (mFocusedGame !is null && mLerpPoint >= 1 && !mFocusedGame.Finished)
			mFocusedGame.Tick(dt);

		if (mLerpPoint == 0 && mFocusedGame !is null)
		{
			@mFocusedGame = null;
			mFX = mFY = 0;
		}
	}

	void Draw(Renderer@ rend)
	{
		if (mFocusedGame !is null && mLerpPoint >= 1)
		{
			rend.View.Center = Vec2(0, 0);

			mFocusedGame.DrawFull(rend);
		}
		else
		{
			Rect gameRect(0, 0, 256, 256);
			const float margin = 25;

			rend.View.Rotation = Math::Lerp(-45.0, 0.0, mLerpPoint);
			rend.View.Center = Math::Lerp(Vec2(mMiniGames.width() * 128, mMiniGames.height() * 128),
				Vec2(mFX * (gameRect.Width + margin) + gameRect.Width / 2,
					 mFY * (gameRect.Height + margin) + gameRect.Height / 2), mLerpPoint);

			rend.View.Size = rend.View.Size * (mMiniGames.width()-mLerpPoint*(mMiniGames.width() - 1 + 0.45));

			for (uint x = 0; x < mMiniGames.width(); ++x)
				for (uint y = 0; y < mMiniGames.height(); ++y)
				{
					Games::IGame@ game = mMiniGames[x, y];
					if (game is null)
						continue;

					gameRect.Left = x * (gameRect.Width + margin);
					gameRect.Top = y * (gameRect.Height + margin);

					bool hover = (@game == @mFocusedGame || gameRect.Contains(rend.PixelToCoords(rend.MousePos)));
					if (hover)
					{
						gameRect.Left -= 10;
						gameRect.Top -= 10;
						gameRect.Width += 20;
						gameRect.Height += 20;

						if (Mouse::IsPressed(Mouse::Button::Left) && mLerpPoint == 0)
						{
							@mFocusedGame = @game;

							mFX = x;
							mFY = y;

							mFocusedGame.StartNewGame();
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
		if (mFocusedGame is null)
		{
			Text choose("Choose your game:");

			choose.Origin = Vec2(choose.LocalBounds.Size.X / 2, 0);
			choose.Position = Vec2(rend.View.Size.X / 2, 15);

			rend.Draw(choose);
		}
		else
		{
			Text curPlay("Currently playing " + mFocusedGame.Name);

			curPlay.Origin = Vec2(curPlay.LocalBounds.Size.X / 2, 0);
			curPlay.Position = Vec2(rend.View.Size.X / 2, 15);

			rend.Draw(curPlay);

			Text back("<< Back");
			back.CharacterSize = 18;

			if (back.GlobalBounds.Contains(rend.MousePos))
			{
				back.Color = Colors::Yellow;

				if (Mouse::IsPressed(Mouse::Button::Left))
				{
					mFocusedGame.EndGame();
					//@mFocusedGame = null;
				}
			}

			rend.Draw(back);
		}
	}

	string Name
	{
		get const { return "Game"; }
	}

	private grid<Games::IGame@> mMiniGames;
	private float mLerpPoint;
	private Games::IGame@ mFocusedGame;
	private uint mFX, mFY;
}

}
