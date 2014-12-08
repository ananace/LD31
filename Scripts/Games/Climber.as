#include "IGame.as"

namespace Games
{

namespace Climber
{

class Platform
{
	Platform()
	{
		Position = Vec2(Math::Random(-255.0, 255.0), -255);
		Width += Math::Random(-32.f, 32.f);
	}

	void Draw(Renderer@ rend, Rect&in area, float scale = 1)
	{
		Shapes::Rectangle Platform(Vec2(Width, Width * Math::PHI));

		Platform.Origin = Platform.LocalBounds.Size / 2;
		Platform.Position = area.Center + Position;
		Platform.OutlineColor = Colors::White;
		Platform.OutlineThickness = 2.5f;
		Platform.FillColor = Colors::Transparent;

		rend.Draw(Platform);
	}

	Rect CollRect {
		get const
		{
			return Rect(Position.X - Width / 2, Position.Y - Width * Math::PHI / 2, Width, Width * Math::PHI);
		}
	}

	bool Jumped = false;
	float Width = 64;
	Vec2 Position;
}

class Player
{
	void Draw(Renderer@ rend, Rect&in area)
	{
		Shapes::Rectangle player(Vec2(20, 45));

		player.Origin = Vec2(10, 45);
		player.Position = area.Center + Position;

		rend.Draw(player);
	}

	Rect CollRect {
		get const
		{
			return Rect(Position.X - 10, Position.Y - 45, 10, 45);
		}
	}

	Vec2 Position = Vec2(0, 250);
	Vec2 Inertia;
}

}

class Climber : IGame
{
	Climber()
	{
		@mHighscore = Highscore();
	}

	void StartNewGame()
	{
		mFinished = false;
		mHasJumped = false;

		mLastCreate = 1;
		mScore = 0;
	}
	void EndGame()
	{
		mFinished = true;
	}
	void Cleanup()
	{
		mEnemies.length = 0;

		mPlayer = Games::Climber::Player();
	}

	void Update(float dt)
	{
		Rect area(-256, -256, 512, 512);

		mAnimTimer += dt;
		mLastQuickCreate -= dt;

		if (mLastQuickCreate <= 0)
		{
			auto@ Platform = Games::Climber::Platform();

			mQuickEnemies.insertLast(Platform);

			mLastQuickCreate += 2.5;
		}

		array<Games::Climber::Platform@> toRemove;
		for (uint i = 0; i < mQuickEnemies.length; ++i)
		{
			auto@ ball = mQuickEnemies[i];

			ball.Position += Vec2(0, 64) * dt;

			bool destroy = false;

			if (ball.Position.Y > area.Top + area.Height)
				destroy = true;

			if (destroy)
				toRemove.insertLast(ball);
		}

		for (uint i = 0; i < toRemove.length; ++i)
		{
			mQuickEnemies.removeAt(mQuickEnemies.findByRef(toRemove[i]));
		}
	}
	void Tick(float dt)
	{
		Rect area(-256, -256, 512, 512);
		mLastCreate -= dt;

		if (mLastCreate <= 0)
		{
			auto@ Platform = Games::Climber::Platform();

			mEnemies.insertLast(Platform);

			mLastCreate += 3;
		}

		bool canJump = false;
		Rect playerColl = mPlayer.CollRect;
		Games::Climber::Platform@ jumper;

		array<Games::Climber::Platform@> toRemove;
		for (uint i = 0; i < mEnemies.length; ++i)
		{
			bool destroy = false;
			auto@ ball = mEnemies[i];

			if (ball.Position.Y > area.Top + area.Height)
			{
				destroy = true;
			}

			Rect intersect;
			ball.Position += Vec2(0, 64) * dt;
			if (ball.CollRect.Intersects(playerColl, intersect))
			{
				if (intersect.Width > intersect.Height && ball.Position.Y > mPlayer.Position.Y)
				{
					canJump = true;
					@jumper = ball;

					if (mPlayer.Inertia.Y > 0)
						mPlayer.Inertia.Y *= -0.01;
				}
				else
					mPlayer.Inertia.X *= -0.5;

				//ball.Position = Vec2(-1,0);
			}

			if (destroy)
				toRemove.insertLast(ball);
		}

		for (uint i = 0; i < toRemove.length; ++i)
		{
			mEnemies.removeAt(mEnemies.findByRef(toRemove[i]));
		}

		Input@ leftRight = Inputs::GetInput(1);
		Input@ jump = Inputs::GetInput(2);

		mPlayer.Inertia = (Vec2(mPlayer.Inertia.X, 0) - Vec2(leftRight.CombinedValue * -32, 0)) + Vec2(0, mPlayer.Inertia.Y + dt / dt * 9.6);
		mPlayer.Position += mPlayer.Inertia * dt;

		if (mPlayer.Position.X - playerColl.Width / 2 < area.Left)
			mPlayer.Position.X = area.Left + playerColl.Width / 2;
		else if (mPlayer.Position.X + playerColl.Width / 2 > area.Left + area.Width)
			mPlayer.Position.X = area.Left + area.Width - playerColl.Width / 2;

		if (mPlayer.Position.Y >= area.Top + area.Height)
		{
			mPlayer.Position.Y = area.Top + area.Height;
			mPlayer.Inertia.Y = 0;
			canJump = true;

			if (mHasJumped)
				EndGame();
		}

		if (jump.Pressed && canJump)
		{
			if (jumper !is null)
			{
				if (!jumper.Jumped)
					mScore++;

				jumper.Jumped = true;
			}

			mHasJumped = true;
			mPlayer.Inertia.Y = -340;
		}
	}

	void DrawQuick(Renderer@ rend, Rect&in area)
	{
		for (uint i = 0; i < mQuickEnemies.length; ++i)
			mQuickEnemies[i].Draw(rend, area);
	}
	void DrawFull(Renderer@ rend, Rect&in area)
	{
		mPlayer.Draw(rend, area);

		for (uint i = 0; i < mEnemies.length; ++i)
			mEnemies[i].Draw(rend, area);
	}

	private Games::Climber::Player mPlayer;
	private array<Games::Climber::Platform@> mEnemies;
	private array<Games::Climber::Platform@> mQuickEnemies;
	private float mLastCreate, mLastQuickCreate;

	bool Finished { get const { return mFinished; } }
	string Name { get const { return "Climbing"; } }
	int Score { get const { return mScore; } }
	int ScoreOrder { get const { return SCORE_HIGHEST; } }
	Player@ Owner { get const { return mOwner; } set { @mOwner = value; } }
	Highscore@ Highscore { get const { return mHighscore; } }

	private Highscore@ mHighscore;
	private float mAnimTimer;
	private int mScore;

	private bool mFinished, mHasJumped;
	private Player@ mOwner;
}

}