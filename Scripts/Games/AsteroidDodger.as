#include "IGame.as"
#include "Asteroid/Common.as"

namespace Games
{

class AsteroidDodger : IGame
{

	void StartNewGame()
	{
		mFinished = false;
		mLastCreate = 0;
	}

	void EndGame()
	{
		mFinished = true;
		mGameShip = Asteroids::Ship();
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
		background.OutlineThickness = 3.5;
		background.OutlineColor = Colors::White;

		rend.Draw(background);

		Asteroids::Ship exampleShip();

		exampleShip.Accel = 1;
		exampleShip.Anim = mAnimTime * 10;
		exampleShip.Position = area.Center;
		exampleShip.Rotation = mAnimTime * 90;

		exampleShip.Draw(rend);
	}

	void DrawFull(Renderer@ rend)
	{
		Rect area(-256, -256, 512, 512);
		Shapes::Rectangle background(area);

		background.FillColor = Colors::Black;

		// Game drawing goes here

		mGameShip.Draw(rend);

		for (uint i = 0; i < mAsteroids.length; ++i)
		{
			mAsteroids[i].Draw(rend);
		}

		// Game drawing goes here

		background.FillColor = Colors::Transparent;
		background.OutlineColor = Colors::White;
		background.OutlineThickness = 2;

		rend.Draw(background);

		area.Left -= 2;
		area.Top -= 2;
		area.Width += 4;
		area.Height += 4;

		background.Rect = area;

		background.FillColor = Colors::Transparent;
		background.OutlineThickness = 128;
		background.OutlineColor = Colors::Black;

		rend.Draw(background);
	}

	bool Finished { get const { return mFinished; } }
	string Name { get const { return "Asteroid Dodger"; } }
	int Score { get const { return TIME_AS_SCORE; } }

	private Asteroids::Ship mGameShip;
	private array<Asteroids::Asteroid@> mAsteroids;

	private bool mFinished;
	private float mAnimTime, mLastCreate;
}

}