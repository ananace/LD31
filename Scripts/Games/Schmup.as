#include "IGame.as"
#include "Asteroid/Common.as"

namespace Games
{

namespace Schmup
{

class Enemy
{
	Enemy()
	{
		Position = Vec2(Math::Random(-255.0, 255.0), -255);
		Rotation = Position.X / 255 * Math::HALF_PI + Math::HALF_PI;
	}

	void Draw(Renderer@ rend, Rect&in area, float scale = 1)
	{
		Shapes::Polygon enemy = {
			Vec2(0, 10),
			Vec2(Width/2, -10),
			Vec2(Width/3, -5),
			Vec2(0, -8),
			Vec2(Width/-3, -5),
			Vec2(Width/-2, -10)
		};

		enemy.Position = area.Center + Position;
		enemy.Rotation = Rotation * Math::R2D - 90;

		rend.Draw(enemy);
	}

	float Width = 64;
	float Rotation;
	Vec2 Position;
}

class Player
{
	void Draw(Renderer@ rend, Rect&in area)
	{
		Shapes::Circle player(25);

		player.Origin = Vec2(25, 25);
		player.Position = area.Center + Position;

		rend.Draw(player);

		Shapes::Rectangle barrel(Vec2(15, 50));

		barrel.Origin = Vec2(7.5, 50);
		barrel.Position = area.Center + Position;
		barrel.Rotation = Rotation;

		rend.Draw(barrel);
	}

	Rect CollRect {
		get const
		{
			return Rect(Position.X - 10, Position.Y - 45, 10, 45);
		}
	}

	Vec2 Position = Vec2(0, 250);
	float Rotation = 0;
}

}

class Schmup : IGame
{
	Schmup()
	{
		@mHighscore = Highscore();
	}

	void StartNewGame()
	{
		mFinished = false;
		mBulletCount = 10;

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
		mBullets.length = 0;

		mPlayer = Games::Schmup::Player();
	}

	void Update(float dt)
	{
		Rect area(-256, -256, 512, 512);

		mAnimTimer += dt;
		mLastQuickCreate -= dt;

		mPlayer.Rotation = sin(mAnimTimer) * 45;

		if (mLastQuickCreate <= 0)
		{
			auto@ enemy = Games::Schmup::Enemy();

			mQuickEnemies.insertLast(enemy);

			mLastQuickCreate += 1.5;
		}

		array<Games::Schmup::Enemy@> toRemove;
		for (uint i = 0; i < mQuickEnemies.length; ++i)
		{
			auto@ ball = mQuickEnemies[i];

			ball.Position += Vec2(cos(ball.Rotation), sin(ball.Rotation)) * 90 * dt;

			bool destroy = false;

			if (ball.Position.X < area.Left ||
				ball.Position.X > area.Left + area.Width ||
				ball.Position.Y > area.Top + area.Height)
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
			auto@ enemy = Games::Schmup::Enemy();

			mEnemies.insertLast(enemy);

			mLastCreate += 2.5;
		}

		array<Games::Schmup::Enemy@> toRemove;
		for (uint i = 0; i < mEnemies.length; ++i)
		{
			bool destroy = false;
			auto@ ball = mEnemies[i];

			for (uint j = 0; j < mBullets.length; ++j)
			{
				Asteroids::Bullet@ bullet = mBullets[j];
				if (bullet.Position.Distance(ball.Position) < bullet.Radius + 20 && bullet.Position.Y >= ball.Position.Y)
				{
					destroy = true;
					mBullets.removeAt(j);

					mScore++;
					break;
				}
			}

			ball.Position += Vec2(cos(ball.Rotation), sin(ball.Rotation)) * 90 * dt;

			if (ball.Position.X < area.Left || ball.Position.Y > area.Left + area.Width || ball.Position.Y > area.Top + area.Height)
				destroy = true;

			if (destroy)
				toRemove.insertLast(ball);
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
			mEnemies.removeAt(mEnemies.findByRef(toRemove[i]));
		}

		for (uint i = 0; i < toRemoveBullet.length; ++i)
		{
			mBullets.removeAt(mBullets.findByRef(toRemoveBullet[i]));

			if (mBullets.length == 0 && mBulletCount <= 0)
				EndGame();
		}

		Input@ fire = Inputs::GetInput(2);

		if (fire.Pressed && !mLastFire && mBulletCount > 0)
		{
			auto@ bullet = Asteroids::Bullet();

			Vec2 bulletDir = Vec2(cos((mPlayer.Rotation-90) * Math::D2R), sin((mPlayer.Rotation-90) * Math::D2R));

			bullet.Position = mPlayer.Position + bulletDir * 50;
			bullet.Inertia = bulletDir * 125;

			mBullets.insertLast(bullet);

			mBulletCount--;
		}
		else if (fire.Pressed && mBullets.length == 0 && mBulletCount <= 0)
			EndGame();

		mLastFire = fire.Pressed;
	}

	void DrawQuick(Renderer@ rend, Rect&in area)
	{
		mPlayer.Draw(rend, area);

		for (uint i = 0; i < mQuickEnemies.length; ++i)
			mQuickEnemies[i].Draw(rend, area);
	}
	void DrawFull(Renderer@ rend, Rect&in area)
	{
		mPlayer.Draw(rend, area);

		for (uint i = 0; i < mBullets.length; ++i)
			mBullets[i].Draw(rend, area);
		for (uint i = 0; i < mEnemies.length; ++i)
			mEnemies[i].Draw(rend, area);
	}

	private Games::Schmup::Player mPlayer;
	private array<Games::Schmup::Enemy@> mEnemies;
	private array<Games::Schmup::Enemy@> mQuickEnemies;
	private array<Asteroids::Bullet@> mBullets;
	private float mLastCreate, mLastQuickCreate;

	bool Finished { get const { return mFinished; } }
	string Name { get const { return "Shoot 'em Up"; } }
	int Score { get const { return mScore; } }
	int ScoreOrder { get const { return SCORE_HIGHEST; } }
	Player@ Owner { get const { return mOwner; } set { @mOwner = value; } }
	Highscore@ Highscore { get const { return mHighscore; } }

	private Highscore@ mHighscore;
	private float mAnimTimer;
	private int mScore, mBulletCount;

	private bool mFinished, mLastFire;
	private Player@ mOwner;
}

}