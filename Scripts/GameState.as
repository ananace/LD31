#include "Games/IGame.as"
#include "Games/Common/Player.as"

#include "Games/Asteroids.as"
#include "Games/AsteroidDodger.as"
#include "Games/LightCycle.as"

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
	case 2:
		return Games::LightCycle();
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
		@mPointShader = Resources::GetShader("Well");

		mMiniGames.resize(3, 3);

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

					int score = 0;
					if (mFocusedGame.Score < 0)
					{
						auto diff = DateTime::Now - mStart;
						score = ((diff.Minute*60) + diff.Second);

						println("After " + score + " seconds");
					}
					else
					{
						score = mFocusedGame.Score;
						println("With " + score + " points");
					}

					auto@ highscore = mFocusedGame.Highscore;
					highscore.PushScore(mCurPlayer, score);
					highscore.Sort(mFocusedGame.ScoreOrder);

					@mFocusedGame.Owner = highscore.Leader;

					int cur = mPlayers.findByRef(mCurPlayer);
					cur = (cur + 1) % mPlayers.length;

					@mCurPlayer = mPlayers[cur];
					winCheck();
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

		float scale = 1.5 * (mMiniGames.width() - mLerpPoint * (mMiniGames.width() - 1 * 0.75));

		rend.View.Size = rend.View.Size * scale;

		if (mFocusedGame !is null && mLerpPoint >= 1)
		{
			gameRect.Left = mFX * (gameRect.Width + margin);
			gameRect.Top = mFY * (gameRect.Height + margin);
			mFocusedGame.DrawFull(rend, gameRect);

			for (int x = -1; x <= 1; ++x)
				for (int y = -1; y <= 1; ++y)
				{
					if ((int(mFX) + x >= 0 && uint(int(mFX) + x) < mMiniGames.width()) &&
						(int(mFY) + y >= 0 && uint(int(mFY) + y) < mMiniGames.height()))
					{
						gameRect.Left = uint(mFX + x) * (gameRect.Width + margin);
						gameRect.Top = uint(mFY + y) * (gameRect.Height + margin);

						if (x == 0 && y == 0)
							continue;

						mMiniGames[uint(mFX + x), uint(mFY + y)].DrawQuick(rend, gameRect);
					}
				}

			gameRect = Rect(Vec2(-4, -4), gameRect.BottomRight);
			gameRect.Width += 8;
			gameRect.Height += 8;

			Shapes::Rectangle rect(gameRect);
			rect.FillColor = Colors::Transparent;
			rect.OutlineColor = Colors::Black;
			rect.OutlineThickness = 128;

			rend.Draw(rect);
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
						if (Mouse::IsPressed(Mouse::Button::XButton2))
						{
							@game.Owner = mPlayers[1];
						}
					}

					if (@game == @mFocusedGame)
					{
						game.DrawFull(rend, gameRect);

						for (int ix = -1; ix < 1; ++ix)
							for (int iy = -1; iy < 1; ++iy)
							{
								gameRect.Left = uint(x + ix) * (gameRect.Width + margin);
								gameRect.Top = uint(y + iy) * (gameRect.Height + margin);

								if (ix == 0 && iy == 0)
									continue;

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

			Rect copy = gameRect;
			copy.Top -= 4;
			copy.Left -= 4;
			copy.Width += 8;
			copy.Height += 8;

			Shapes::Rectangle rect(copy);
			rect.FillColor = Colors::Transparent;
			rect.OutlineColor = Colors::Black;
			rect.OutlineThickness = 128;
			rend.Draw(rect);

			Shapes::Circle point;
			point.FillColor = Colors::Transparent;
			point.Radius = 64;
			point.Origin = Vec2(64, 64);

			float testScale = (rend.View.Size / rend.Size).Length;

			Color temp = mCurPlayer.Color;

			for (int i = 5; i > 0; --i)
			{
				Vec2 direction(cos(mPointAng - (5-i) * 0.02), sin(mPointAng - (5-i) * 0.02));
				point.Position = gameRect.Constrain(Vec2(mMiniGames.width(), mMiniGames.height()) * 256 + direction * mMiniGames.width() * 512) + direction * (point.Radius + 16);
				
				temp.A = uint8(255 * (i / 6.f));
				//point.FillColor = temp;

				Vec2 tempPos = rend.CoordsToPixel(Vec2(point.Position.X, point.Position.Y));
				mPointShader.SetParameter("center", tempPos.X, rend.Size.Y - tempPos.Y, 0, 96 / testScale);
				mPointShader.SetParameter("color", temp);

				rend.Draw(point, mPointShader);
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

			score.CharacterSize = 20;

			score.Origin = Vec2(score.LocalBounds.Size.X / 2, 0);
			score.Position = Vec2(rend.View.Size.X / 2, 15);

			score.Color = Colors::Black;

			score.Move(2, 2);
			rend.Draw(score);
			score.Move(-2, -2);
			score.Color = Colors::White;
			rend.Draw(score);
			score.Move(-0.1, -0.1);
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

	private void winCheck()
	{
		for (uint x = 0; x < mMiniGames.width(); ++x)
			for (uint y = 0; y < mMiniGames.height(); ++y)
			{
				if ((x > 0 && x < mMiniGames.width() - 1) &&
					(y > 0 && y < mMiniGames.height() - 1))
					continue;

				uint checkX = x, checkY = y;
				Player@ checkPlayer = mMiniGames[checkX, checkY].Owner;

				if (checkPlayer is null)
					continue;

				for (uint8 dir = 0; dir < 8; ++dir)
				{
					uint length = 0;
					checkX = x; checkY = y;

					while (true)
					{
						switch(dir)
						{
						case 0: checkY--; break;
						case 1: checkY--; checkX--; break;
						case 2: checkX--; break;
						case 3: checkX--; checkY++; break;
						case 4: checkY++; break;
						case 5: checkY++; checkX++; break;
						case 6: checkX++; break;
						case 7: checkX++; checkY--; break;
						}

						if (checkX < 0 || checkY < 0 || checkX >= mMiniGames.width() || checkY >= mMiniGames.height())
							break;

						Player@ owner = mMiniGames[checkX, checkY].Owner;
						if (owner is null || @owner != @checkPlayer)
							break;

						length++;

						if (length == mMiniGames.width() - 1)
						{
							println(checkPlayer.Name + " just won the game");
							mStateMan.PopState();
							return;
						}
					}
				}
			}
	}

	string Name
	{
		get const { return "Game"; }
	}

	private StateMachine@ mStateMan;

	private DateTime mStart;

	private Shader@ mPointShader;
	private Player@ mCurPlayer;
	private array<Player@> mPlayers;
	private grid<Games::IGame@> mMiniGames;
	private float mLerpPoint, mPointAng;
	private Games::IGame@ mFocusedGame;
	private uint mFX, mFY;
}

}

