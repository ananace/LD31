#include "IGame.as"
#include "Asteroid/Common.as"

namespace Games
{

class Asteroids : IGame
{
	Asteroids()
	{
		mAnimTime = Math::Random(0.f, Math::DOUBLE_PI);
	}

	void StartNewGame()
	{
		mFinished = false;
		mLastCreate = 0;
		mScore = 0;
	}

	void EndGame()
	{
		mFinished = true;
	}

	void Cleanup()
	{
		mGameShip = Asteroids::Ship();

		mAsteroids.length = 0;
		mBullets.length = 0;
	}

	void Update(float dt)
	{
		mAnimTime += dt;
		mLastQuickFire = max(0, mLastQuickFire - dt);

		mGameShip.Anim = mAnimTime * 25;

		for (uint i = 0; i < mQuickBullets.length; ++i)
		{
			auto@ bullet = mQuickBullets[i];

			bullet.Position += bullet.Inertia * dt;
		}
	}

	void Tick(float dt)
	{
		mLastCreate += dt;
		mLastFire = max(-5, mLastFire - dt);

		Rect area(-256, -256, 512, 512);
		
		if (mLastCreate > 2)
		{
			mLastCreate -= 2;

			mAsteroids.insertLast(Asteroids::Asteroid(area));
		}

		array<Asteroids::Asteroid@> toRemove;
		for (uint i = 0; i < mAsteroids.length; ++i)
		{
			Asteroids::Asteroid@ asteroid = @mAsteroids[i];

			bool destroy = false;

			for (uint j = 0; j < mBullets.length; ++j)
			{
				Asteroids::Bullet@ bullet = mBullets[j];
				if (bullet.Position.Distance(asteroid.Position) < asteroid.Radius + bullet.Radius)
				{
					destroy = true;
					mBullets.removeAt(j);

					mScore++;

					float radius = asteroid.Radius / 1.5;
					if (radius < bullet.Radius * 2)
						break;

					for (int k = 0; k < Math::Random(2, 6); ++k)
					{
						auto@ fragment = Asteroids::Asteroid(area, radius);
						fragment.Position = asteroid.Position + fragment.Inertia;
						mAsteroids.insertLast(fragment);
					}
					break;
				}
			}

			if (!destroy)
			{
				if (mGameShip.Position.Distance(asteroid.Position) < asteroid.Radius)
				{
					EndGame();
					return;
				}

				asteroid.Position += asteroid.Inertia * dt;
				asteroid.Rotation += asteroid.RotationMult * dt;

				if (asteroid.Position.X < area.Left - asteroid.Radius)
					destroy = true;
				else if (asteroid.Position.X > area.Left + area.Width + asteroid.Radius)
					destroy = true;

				if (asteroid.Position.Y < area.Top - asteroid.Radius)
					destroy = true;
				else if (asteroid.Position.Y > area.Top + area.Height + asteroid.Radius)
					destroy = true;
			}

			if (destroy)
				toRemove.insertLast(asteroid);
		}

		array<Asteroids::Bullet@> toRemoveBullet;
		for (uint i = 0; i < mBullets.length; ++i)
		{
			Asteroids::Bullet@ bullet = mBullets[i];

			bullet.Position += bullet.Inertia * dt;

			bool destroy = false;
			if (bullet.Position.X < area.Left - bullet.Radius)
				destroy = true;
			else if (bullet.Position.X > area.Left + area.Width + bullet.Radius)
				destroy = true;

			if (bullet.Position.Y < area.Top - bullet.Radius)
				destroy = true;
			else if (bullet.Position.Y > area.Top + area.Height + bullet.Radius)
				destroy = true;

			if (destroy)
				toRemoveBullet.insertLast(bullet);
		}

		for (uint i = 0; i < toRemove.length; ++i)
		{
			mAsteroids.removeAt(mAsteroids.findByRef(toRemove[i]));
		}
		for (uint i = 0; i < toRemoveBullet.length; ++i)
		{
			mBullets.removeAt(mBullets.findByRef(toRemoveBullet[i]));
		}

		Input@ acc = Inputs::GetInput(0);
		Input@ turn = Inputs::GetInput(1);
		Input@ fire = Inputs::GetInput(2);

		if (fire.Pressed && !mLastFirePressed && mLastFire <= 0)
		{
			const float BulletSpeed = 85;

			mLastFire += 1;

			auto@ bullet = Asteroids::Bullet();

			Vec2 bulletDir = Vec2(cos(mGameShip.Rotation * Math::D2R - Math::HALF_PI),
									  sin(mGameShip.Rotation * Math::D2R - Math::HALF_PI));
			bullet.Position = mGameShip.Position + bulletDir * 25;
			bullet.Inertia = mGameShip.Inertia + bulletDir * BulletSpeed;

			mBullets.insertLast(bullet);
		}

		mLastFirePressed = fire.Pressed;

		mGameShip.Accel = acc.Value;
		if (mGameShip.Accel > 0)
			mGameShip.Inertia += Vec2(cos(mGameShip.Rotation * Math::D2R - Math::HALF_PI) * acc.Value,
									  sin(mGameShip.Rotation * Math::D2R - Math::HALF_PI) * acc.Value);

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

		exampleShip.Accel = 1;
		exampleShip.Anim = mAnimTime * 10;
		exampleShip.Rotation = mAnimTime * 90;

		if (mLastQuickFire <= 0)
		{
			mLastQuickFire += 1 + Math::Random(-0.25, 0.35);

			auto@ bullet = Asteroids::Bullet();

			Vec2 bulletDir = Vec2(cos(exampleShip.Rotation * Math::D2R - Math::HALF_PI),
									  sin(exampleShip.Rotation * Math::D2R - Math::HALF_PI));
			bullet.Position = exampleShip.Position + bulletDir * 25;
			bullet.Inertia = exampleShip.Inertia + bulletDir * 85;

			mQuickBullets.insertLast(bullet);
		}

		for (uint i = 0; i < mQuickBullets.length; ++i)
		{
			mQuickBullets[i].Draw(rend, area);
		}

		exampleShip.Draw(rend, area, 2.f);

		array<Asteroids::Bullet@> toRemoveBullet;
		for (uint i = 0; i < mQuickBullets.length; ++i)
		{
			Asteroids::Bullet@ bullet = mQuickBullets[i];

			Vec2 realPos = area.Center + bullet.Position;
			bool destroy = false;
			if (realPos.X < area.Left - bullet.Radius)
				destroy = true;
			else if (realPos.X > area.Left + area.Width + bullet.Radius)
				destroy = true;

			if (realPos.Y < area.Top - bullet.Radius)
				destroy = true;
			else if (realPos.Y > area.Top + area.Height + bullet.Radius)
				destroy = true;

			if (destroy)
				toRemoveBullet.insertLast(bullet);
		}

		for (uint i = 0; i < toRemoveBullet.length; ++i)
		{
			mQuickBullets.removeAt(mQuickBullets.findByRef(toRemoveBullet[i]));
		}

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

		for (uint i = 0; i < mBullets.length; ++i)
		{
			mBullets[i].Draw(rend, area);
		}

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
	string Name { get const { return "Asteroids"; } }
	int Score { get const { return mScore; } }
	int ScoreOrder { get const { return SCORE_HIGHEST; } }
	Player@ Owner { get const { return mOwner; } set { @mOwner = value; } }

	private int mScore;

	private Player@ mOwner;

	private Asteroids::Ship mGameShip;
	private array<Asteroids::Asteroid@> mAsteroids;
	private array<Asteroids::Bullet@> mBullets, mQuickBullets;

	private bool mFinished, mLastFirePressed;
	private float mAnimTime, mLastCreate, mLastFire, mLastQuickFire;
}

}