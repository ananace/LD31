#include "Games/IGame.as"

#include "Games/Asteroids.as"
#include "Games/AsteroidDodger.as"

#include "IState.as"

float min(float a, float b) { return (a < b ? a : b); }
float max(float a, float b) { return (a > b ? a : b); }

namespace States
{

Games::IGame@ CreateGame()
{
	switch(Math::Random(0, 1))
	{
	case 0:
		return Games::Asteroids();
	case 1:
		return Games::AsteroidDodger();
	}

	return null;
}

class GameState : IState
{
	GameState()
	{
		mMiniGames.resize(4, 4);

		for (uint x = 0; x < mMiniGames.width(); ++x)
			for (uint y = 0; y < mMiniGames.height(); ++y)
				@mMiniGames[x, y] = CreateGame();
	}

	void Init(StateMachine@ man)
	{
		@mStateMan = @man;
	}

	void Update(float dt)
	{
		mPointAng += dt * Math::PHI;

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

					Vec2 mousePos = rend.PixelToCoords(rend.MousePos);
					bool hover = (@game == @mFocusedGame || gameRect.Contains(mousePos));
					if (hover)
					{
						gameRect.Left -= 10;
						gameRect.Top -= 10;
						gameRect.Width += 20;
						gameRect.Height += 20;

						if (Mouse::IsPressed(Mouse::Button::Left) && mLerpPoint == 0 && !Inputs::Disabled)
						{
							@mFocusedGame = @game;

							mFX = x;
							mFY = y;

							mFocusedGame.StartNewGame();
							mStart = DateTime::Now;
						}
					}

					//println("[" + x + ", " + y + "] = " + gameRect.Left + ", " + gameRect.Top);

					game.DrawQuick(rend, gameRect);

					if (hover)
					{
						gameRect.Width -= 20;
						gameRect.Height -= 20;

						if (mLerpPoint == 0)
						{
							Text title(game.Name);
							title.CharacterSize = 30 + (mMiniGames.width() * 10);
							title.Rotation = -45;
							title.Origin = title.LocalBounds.Size / 2;
							title.Position = mousePos;
							rend.Draw(title);
						}
					}
				}

			gameRect = Rect(Vec2(0, 0), gameRect.BottomRight);

			Shapes::Circle point;
			point.Radius = 64;
			point.Origin = Vec2(64,64);

			Color temp = Colors::White;

			for (int i = 5; i > 0; --i)
			{
				Vec2 direction(cos(mPointAng - (5-i) * 0.02), sin(mPointAng - (5-i) * 0.02));
				point.Position = gameRect.Constrain(direction * mMiniGames.width() * 512) + direction * (point.Radius + 16);
				
				temp.A = uint8(255 * (i / 6.f));
				point.FillColor = temp;

				rend.Draw(point);
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

			Text back("<< Back");
			back.CharacterSize = 18;

			if (back.GlobalBounds.Contains(rend.MousePos))
			{
				back.Color = Colors::Yellow;

				if (Mouse::IsPressed(Mouse::Button::Left) && !Inputs::Disabled)
				{
					mStateMan.PopState();
				}
			}

			rend.Draw(back);
		}
		else
		{
			Text score;
			if (mFocusedGame.Score < 0)
			{
				DateTime time = (DateTime::Now - mStart);
				score.String = "Current time: " +
					(time.Minute < 10 ? "0" : "") + time.Minute + ":" +
					(time.Second < 10 ? "0" : "") + time.Second;
			}
			else
				score.String = "Current score: " + mFocusedGame.Score;

			score.CharacterSize = 18;

			score.Origin = Vec2(score.LocalBounds.Size.X / 2, 0);
			score.Position = Vec2(rend.View.Size.X / 2, 15);

			rend.Draw(score);


			Text back("<< Back");
			back.CharacterSize = 18;

			if (back.GlobalBounds.Contains(rend.MousePos))
			{
				back.Color = Colors::Yellow;

				if (Mouse::IsPressed(Mouse::Button::Left) && !Inputs::Disabled)
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

	private StateMachine@ mStateMan;

	private DateTime mStart;

	private grid<Games::IGame@> mMiniGames;
	private float mLerpPoint, mPointAng;
	private Games::IGame@ mFocusedGame;
	private uint mFX, mFY;
}

}
