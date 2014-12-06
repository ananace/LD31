#include "IGame.as"
#include "Asteroid/Common.as"

namespace Games
{

class AsteroidDodger : IGame
{
	AsteroidDodger()
	{
		mAnimTime = Math::Random(0.f, Math::DOUBLE_PI);
		@mHighscore = Highscore();
	}

	void StartNewGame()
	{
		mGameShip = Asteroids::Ship();
		mGameShip.ExhaustColor = Color(255, 196, 16);

		mFinished = false;
		mLastCreate = 0;
	}

	void EndGame()
	{
		mFinished = true;
	}

	void Cleanup()
	{
		mAsteroids.length = 0;
	}

	void Update(float dt)
	{
		mAnimTime += dt;

		mGameShip.Anim = mAnimTime * 25;
	}

	void Tick(float dt)
	{
		mLastCreate += dt;

		Rect area(-256, -256, 512, 512);
		
		if (mLastCreate > 1)
		{
			mLastCreate -= 1;

			Asteroids::Asteroid@ asteroid = Asteroids::Asteroid(area);
			asteroid.Inertia *= 2.5;
			mAsteroids.insertLast(asteroid);
		}

		array<Asteroids::Asteroid@> toRemove;
		for (uint i = 0; i < mAsteroids.length; ++i)
		{
			Asteroids::Asteroid@ asteroid = @mAsteroids[i];

			if (mGameShip.Position.Distance(asteroid.Position) < asteroid.Radius)
			{
				EndGame();
				return;
			}

			asteroid.Position += asteroid.Inertia * dt;
			asteroid.Rotation += asteroid.RotationMult * dt;

			bool destroy = false;

			if (asteroid.Position.X < area.Left - asteroid.Radius)
				destroy = true;
			else if (asteroid.Position.X > area.Left + area.Width + asteroid.Radius)
				destroy = true;

			if (asteroid.Position.Y < area.Top - asteroid.Radius)
				destroy = true;
			else if (asteroid.Position.Y > area.Top + area.Height + asteroid.Radius)
				destroy = true;

			if (destroy)
				toRemove.insertLast(asteroid);
		}

		for (uint i = 0; i < toRemove.length; ++i)
		{
			mAsteroids.removeAt(mAsteroids.findByRef(toRemove[i]));
		}

		Input@ acc = Inputs::GetInput(0);
		Input@ turn = Inputs::GetInput(1);

		mGameShip.Accel = acc.Value;
		if (mGameShip.Accel > 0)
			mGameShip.Inertia += Vec2(cos(mGameShip.Rotation * Math::D2R - Math::HALF_PI),
									  sin(mGameShip.Rotation * Math::D2R - Math::HALF_PI));

		mGameShip.Position += mGameShip.Inertia * dt;
		mGameShip.Rotation += turn.CombinedValue * dt * 90;

		const float radius = 20;

		if (mGameShip.Position.X < area.Left - radius)
			mGameShip.Position.X = area.Left + area.Width + radius;
		else if (mGameShip.Position.X > area.Left + area.Width + radius)
			mGameShip.Position.X = area.Left - radius;

		if (mGameShip.Position.Y < area.Top - radius)
			mGameShip.Position.Y = area.Top + area.Height + radius;
		else if (mGameShip.Position.Y > area.Top + area.Height + radius)
			mGameShip.Position.Y = area.Top - radius;
	}

	void DrawQuick(Renderer@ rend, Rect&in area)
	{
		Shapes::Rectangle background(area);

		background.FillColor = Colors::Black;
		rend.Draw(background);

		Color temp = Colors::Transparent;		
		if (mOwner !is null)
		{
			temp = mOwner.Color;
			temp.A = 96;
		}

		background.FillColor = temp;
		background.OutlineThickness = 3.5;
		background.OutlineColor = Colors::White;

		rend.Draw(background);

		Asteroids::Ship exampleShip();

		exampleShip.Accel = 0.9 + cos(mAnimTime * 4) / 10;
		exampleShip.Anim = mAnimTime * 10;
		exampleShip.ExhaustColor = Color(255, 196, 16);
		exampleShip.Rotation = mAnimTime * 90;

		exampleShip.Draw(rend, area, 2);

		area.Left -= 3.5;
		area.Top -= 3.5;
		area.Width += 7;
		area.Height += 7;

		background.Rect = area;

		background.FillColor = Colors::Transparent;
		background.OutlineThickness = 16;
		background.OutlineColor = Colors::Black;

		rend.Draw(background);
	}

	void DrawFull(Renderer@ rend, Rect&in area)
	{
		Shapes::Rectangle background(area);

		mGameShip.Draw(rend, area);

		for (uint i = 0; i < mAsteroids.length; ++i)
		{
			mAsteroids[i].Draw(rend, area);
		}

		background.FillColor = Colors::Transparent;
		background.OutlineColor = Colors::White;
		background.OutlineThickness = 3.5;

		rend.Draw(background);

		area.Left -= 3.5;
		area.Top -= 3.5;
		area.Width += 7;
		area.Height += 7;

		background.Rect = area;

		background.FillColor = Colors::Transparent;
		background.OutlineThickness = 18;
		background.OutlineColor = Colors::Black;

		rend.Draw(background);
	}

	bool Finished { get const { return mFinished; } }
	string Name { get const { return "Asteroid Dodger"; } }
	int Score { get const { return TIME_AS_SCORE; } }
	int ScoreOrder { get const { return SCORE_HIGHEST; } }
	Player@ Owner { get const { return mOwner; } set { @mOwner = value; } }
	Highscore@ Highscore { get const { return mHighscore; } }

	private Highscore@ mHighscore;
	private Player@ mOwner;
	private Asteroids::Ship mGameShip;
	private array<Asteroids::Asteroid@> mAsteroids;

	private bool mFinished;
	private float mAnimTime, mLastCreate;
}

}