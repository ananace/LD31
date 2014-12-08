#include "IGame.as"

namespace Games
{

namespace DodgeBall
{

class Ball
{
	Ball()
	{
		@mBallShader = Resources::GetShader("Well");
		RotationMult = Math::Random(-1.75, 1.75);

		float randAng = Math::Random(-1, 1) * Math::PI;
		Inertia = Vec2(cos(randAng), sin(randAng)) * Math::Random(20.0, 65.0);
	}

	void Draw(Renderer@ rend, Rect&in area, float scale = 1)
	{
		Shapes::Circle circ(Radius);

		circ.SetOrigin(Radius, Radius);
		circ.Position = area.Center + Position;

		circ.FillColor = Colors::Transparent;
		circ.OutlineColor = Colors::White;
		circ.OutlineThickness = 2.f;

		circ.SetScale(scale);

		if (Shaders::Available)
		{
			float testScale = (rend.View.Size / rend.Size).Length;

			Vec2 tempPos = rend.CoordsToPixel(circ.Position + Vec2(cos(Rotation), sin(Rotation))*Radius/2);
			mBallShader.SetParameter("center", tempPos.X, rend.Size.Y - tempPos.Y, 0, Radius*2 / testScale);
			mBallShader.SetParameter("color", Colors::White);

			rend.Draw(circ, mBallShader);
		}
		else
			rend.Draw(circ);
	}

	float Radius = 10;
	float Rotation, RotationMult;
	Vec2 Position;
	Vec2 Inertia;

	private Shader@ mBallShader;
}

class Player
{
	void Draw(Renderer@ rend, Rect&in area)
	{
		Shapes::Rectangle player(Vec2(20, 45));

		if (Ducking)
		{
			player.Rotation = 90;
			player.Origin = Vec2(20, 22.5);
		}
		else
			player.Origin = Vec2(10, 45);
		player.Position = area.Center + Position;

		rend.Draw(player);
	}

	Rect CollRect {
		get const
		{
			if (Ducking)
				return Rect(Position.X - 22.5, Position.Y - 10, 45, 10);
			else
				return Rect(Position.X - 10, Position.Y - 45, 10, 45);
		}
	}

	bool Ducking = false;
	Vec2 Position = Vec2(0, 250);
	Vec2 Inertia;
	float RunSpeed = 4;
}

}

class DodgeBall : IGame
{
	DodgeBall()
	{
		@mHighscore = Highscore();
	}

	void StartNewGame()
	{
		mFinished = false;

		mLastCreate = 1;
	}
	void EndGame()
	{
		mFinished = true;
	}
	void Cleanup()
	{
		mBalls.length = 0;

		mPlayer = Games::DodgeBall::Player();
	}

	void Update(float dt)
	{
		Rect area(-256, -256, 512, 512);
		mAnimTimer += dt;
		mLastQuickCreate -= dt;

		if (mLastQuickCreate <= 0)
		{
			Games::DodgeBall::Ball@ newBall = Games::DodgeBall::Ball();

			newBall.Radius = 12.5;
			mQuickBalls.insertLast(newBall);

			mLastQuickCreate += 2.5;
		}

		array<Games::DodgeBall::Ball@> toRemove;
		for (uint i = 0; i < mQuickBalls.length; ++i)
		{
			auto@ ball = mQuickBalls[i];

			for (uint j = 0; j < mQuickBalls.length; ++j)
			{
				auto@ ball2 = mQuickBalls[j];

				Vec2 diff = ball2.Position - ball.Position;
				if (diff.Length < ball.Radius+ball2.Radius)
				{
					float pen = ball.Radius+ball2.Radius - diff.Length;
					ball.Inertia += diff * -pen;
					ball2.Inertia += diff * pen;
				}
			}

			ball.Inertia += Vec2(0, 24) * dt;
			ball.Position += ball.Inertia * dt;
			ball.Rotation += dt * ball.RotationMult;

			bool destroy = false;
			if ((ball.Position.X + ball.Radius < area.Left && ball.Inertia.X < 0) || 
				(ball.Position.X - ball.Radius > area.Left + area.Width && ball.Inertia.X > 0))
				destroy = true;

			if (ball.Position.Y + ball.Radius > area.Top + area.Height && ball.Inertia.Y > 0)
				ball.Inertia.Y *= -0.95;
			else if (ball.Position.Y - ball.Radius < area.Top)
				destroy = true;

			if (destroy)
				toRemove.insertLast(ball);
		}

		for (uint i = 0; i < toRemove.length; ++i)
		{
			mQuickBalls.removeAt(mQuickBalls.findByRef(toRemove[i]));
		}
	}
	void Tick(float dt)
	{
		Rect area(-256, -256, 512, 512);
		mLastCreate -= dt;

		if (mLastCreate <= 0)
		{
			Games::DodgeBall::Ball newBall;

			mBalls.insertLast(newBall);

			mLastCreate += 5;
		}

		Rect playerColl = mPlayer.CollRect;
		for (uint i = 0; i < mBalls.length; ++i)
		{
			auto@ ball = mBalls[i];

			for (uint j = 0; j < mBalls.length; ++j)
			{
				auto@ ball2 = mBalls[j];

				Vec2 diff = ball2.Position - ball.Position;
				if (diff.Length < ball.Radius+ball2.Radius)
				{
					float pen = ball.Radius+ball2.Radius - diff.Length;
					ball.Inertia += diff * -pen * 0.5;
					ball2.Inertia += diff * pen * 0.5;
				}
			}

			ball.Rotation += dt * ball.RotationMult;

			if ((ball.Position.X - ball.Radius < area.Left && ball.Inertia.X < 0) || 
				(ball.Position.X + ball.Radius > area.Left + area.Width && ball.Inertia.X > 0))
			{
				ball.Inertia.X *= -1;

				if (Math::Random(0, 1) == 0)
					ball.RotationMult *= -1;
			}
			if (ball.Position.Y + ball.Radius > area.Top + area.Height && ball.Inertia.Y > 0)
			{
				ball.Position.Y += 1;
				ball.Inertia.Y *= -0.95;
				ball.RotationMult *= Math::Random(0.95, 1.05);
			}

			ball.Inertia += Vec2(0, 24) * dt;
			ball.Position += ball.Inertia * dt;

			Vec2 constr = playerColl.Constrain(ball.Position);
			if (constr.Distance(ball.Position) < ball.Radius)
				EndGame();
		}

		Input@ jump = Inputs::GetInput(2);
		Input@ down = Inputs::GetInput(3);
		Input@ leftRight = Inputs::GetInput(1);

		if (jump.Pressed && mPlayer.Inertia.Y == 0 && mPlayer.Position.Y >= area.Top + area.Height - 2)
			mPlayer.Inertia.Y = -96 * (mPlayer.Ducking ? 0.75 : 1);
		mPlayer.Ducking = down.Pressed;

		mPlayer.Inertia += Vec2(0, 96) * dt;
		if (mPlayer.Position.Y >= area.Top + area.Height && mPlayer.Inertia.Y > 0)
			mPlayer.Inertia.Y = 0;
		mPlayer.Position += Vec2(leftRight.CombinedValue * mPlayer.RunSpeed * (mPlayer.Ducking ? 0.5 : 1), 0) + mPlayer.Inertia * dt;

		if (mPlayer.Position.X - playerColl.Width / 2 < area.Left)
			mPlayer.Position.X = area.Left + playerColl.Width / 2;
		else if (mPlayer.Position.X + playerColl.Width / 2 > area.Left + area.Width)
			mPlayer.Position.X = area.Left + area.Width - playerColl.Width / 2;
	}

	void DrawQuick(Renderer@ rend, Rect&in area)
	{
		for (uint i = 0; i < mQuickBalls.length; ++i)
			mQuickBalls[i].Draw(rend, area);
	}
	void DrawFull(Renderer@ rend, Rect&in area)
	{
		mPlayer.Draw(rend, area);

		for (uint i = 0; i < mBalls.length; ++i)
			mBalls[i].Draw(rend, area);
	}

	private Games::DodgeBall::Player mPlayer;
	private array<Games::DodgeBall::Ball> mBalls;
	private array<Games::DodgeBall::Ball@> mQuickBalls;
	private float mLastCreate, mLastQuickCreate;

	bool Finished { get const { return mFinished; } }
	string Name { get const { return "Dodge 'em"; } }
	int Score { get const { return TIME_AS_SCORE; } }
	int ScoreOrder { get const { return SCORE_HIGHEST; } }
	Player@ Owner { get const { return mOwner; } set { @mOwner = value; } }
	Highscore@ Highscore { get const { return mHighscore; } }

	private Highscore@ mHighscore;
	private float mAnimTimer;

	private bool mFinished;
	private Player@ mOwner;
}

}