#include "Games/IGame.as"
#include "Games/Common/Player.as"

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
		array<Player@> players = {
			Player("Dick Dastardly", Colors::Red),
			Player("Bruce", Colors::Yellow),
			Player("Rob D. Robot", Colors::Blue)
		};

		mPlayers = players;
		@mCurPlayer = mPlayers[0];

		mMiniGames.resize(5, 5);

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

			for (int x = -1; x <= 1; ++x)
				for (int y = -1; y <= 1; ++y)
				{
					if (x == 0 && y == 0)
						continue;

					if ((int(mFX) + x >= 0 && uint(int(mFX) + x) < mMiniGames.width()) &&
						(int(mFY) + y >= 0 && uint(int(mFY) + y) < mMiniGames.height()))
					{
						mMiniGames[mFX + x, mFY + y].Update(dt);
					}
				}
		}
	}

	void Tick(float dt)
	{
		if (mFocusedGame !is null && mLerpPoint >= 1)
		{
			if (!mFocusedGame.Finished)
			{
				mFocusedGame.Tick(dt);

				if (mFocusedGame.Finished)
				{
					println("Game ended");

					if (mFocusedGame.Score < 0)
					{
						auto diff = DateTime::Now - mStart;

						println("After " + ((diff.Minute*60) + diff.Second) + " seconds");
					}
					else
						println("With " + mFocusedGame.Score + " points");
				}
			}
		}

		if (mLerpPoint == 0 && mFocusedGame !is null)
		{
			mFocusedGame.Cleanup();

			@mFocusedGame = null;
			mFX = mFY = 0;
		}
	}

	void Draw(Renderer@ rend)
	{
		Rect gameRect(0, 0, 512, 512);
		const float margin = 25;

		rend.View.Rotation = Math::Lerp(-45.0, 0.0, mLerpPoint);
		rend.View.Center = Math::Lerp(Vec2(mMiniGames.width() * 256, mMiniGames.height() * 256),
			Vec2(mFX * (gameRect.Width + margin) + gameRect.Width / 2,
				 mFY * (gameRect.Height + margin) + gameRect.Height / 2), mLerpPoint);

		rend.View.Size = rend.View.Size * 1.5 *
			(mMiniGames.width() - mLerpPoint * (mMiniGames.width() - 1));

		if (mFocusedGame !is null && mLerpPoint >= 1)
		{
			gameRect.Left = mFX * (gameRect.Width + margin);
			gameRect.Top = mFY * (gameRect.Height + margin);
			mFocusedGame.DrawFull(rend, gameRect);

			for (int x = -1; x <= 1; ++x)
				for (int y = -1; y <= 1; ++y)
				{
					if (x == 0 && y == 0)
						continue;

					gameRect.Left = uint(mFX + x) * (gameRect.Width + margin);
					gameRect.Top = uint(mFY + y) * (gameRect.Height + margin);

					if ((int(mFX) + x >= 0 && uint(int(mFX) + x) < mMiniGames.width()) &&
						(int(mFY) + y >= 0 && uint(int(mFY) + y) < mMiniGames.height()))
					{
						mMiniGames[uint(mFX + x), uint(mFY + y)].DrawQuick(rend, gameRect);
					}
				}
		}
		else
		{
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
						if (Mouse::IsPressed(Mouse::Button::Left) && mLerpPoint == 0 && !Inputs::Disabled)
						{
							@mFocusedGame = @game;

							mFX = x;
							mFY = y;

							mFocusedGame.StartNewGame();
							mStart = DateTime::Now;
						}
						if (Mouse::IsPressed(Mouse::Button::XButton1))
						{
							@game.Owner = mCurPlayer;
						}
					}

					if (@game == @mFocusedGame)
					{
						game.DrawFull(rend, gameRect);

						for (int ix = -1; ix < 1; ++ix)
							for (int iy = -1; iy < 1; ++iy)
							{
								if (ix == 0 && iy == 0)
									continue;

								gameRect.Left = uint(x + ix) * (gameRect.Width + margin);
								gameRect.Top = uint(y + iy) * (gameRect.Height + margin);

								if ((int(x) + ix >= 0 && uint(int(x) + ix) < mMiniGames.width()) &&
									(int(y) + iy >= 0 && uint(int(y) + iy) < mMiniGames.height()))
								{
									mMiniGames[uint(x + ix), uint(y + iy)].DrawQuick(rend, gameRect);
								}
							}
					}
					else
						game.DrawQuick(rend, gameRect);

					if (hover)
					{
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
				point.Position = gameRect.Constrain(Vec2(mMiniGames.width(), mMiniGames.height()) * 256 + direction * mMiniGames.width() * 512) + direction * (point.Radius + 16);
				
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
			Text playList("Players:");

			playList.CharacterSize = 20;
			playList.Move(8, 32);

			rend.Draw(playList);
			
			playList.Move(24, 6);
			Shapes::Rectangle rekt(Rect(8, 38, 16.f, 16.f));
			rekt.Origin = Vec2(-4, -4);

			rekt.OutlineColor = Colors::White;
			rekt.OutlineThickness = 1;

			for (uint i = 0; i < mPlayers.length; ++i)
			{
				rekt.Move(0, 26);
				playList.Move(0, 26);
				Player@ p = mPlayers[i];

				rekt.FillColor = p.Color;
				rend.Draw(rekt);

				playList.String = p.Name;

				rend.Draw(playList);
			}

			Text choose(mCurPlayer.Name + "'s turn, choose your game:");

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

	private Player@ mCurPlayer;
	private array<Player@> mPlayers;
	private grid<Games::IGame@> mMiniGames;
	private float mLerpPoint, mPointAng;
	private Games::IGame@ mFocusedGame;
	private uint mFX, mFY;
}

}
