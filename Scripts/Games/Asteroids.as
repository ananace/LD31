#include "IGame.as"

namespace Games
{

namespace Asteroids
{
	class Ship
	{
		void Draw(Renderer@ rend)
		{
			Shapes::Polygon ship = {
				Vec2(0, -20),
				Vec2(10, 10),
				Vec2(0, 5),
				Vec2(-10, 10)
			};

			ship.Position = Position;

			ship.FillColor = Colors::Transparent;
			ship.OutlineColor = Colors::White;
			ship.OutlineThickness = 2;

			ship.Rotation = Rotation;

			rend.Draw(ship);

			if (Accel)
			{
				Shapes::Polygon exhaust = {
					Vec2(10, 0),
					Vec2(10, 10),
					Vec2(3, 2),
					Vec2(0, 12),
					Vec2(-3, 1),
					Vec2(-10, 10),
					Vec2(-10, 0),
					Vec2(0, -5)
				};

				exhaust.SetPoint(1, Vec2(10 + sin(Anim * 2.15), 10));
				exhaust.SetPoint(2, Vec2(3, 2 + sin(Anim * 1.25)));
				exhaust.SetPoint(3, Vec2(sin(Anim * 1.75), 12));
				exhaust.SetPoint(4, Vec2(-3, 2 + cos(Anim)));
				exhaust.SetPoint(5, Vec2(-10 + cos(Anim * 2.25), 10));

				exhaust.SetOrigin(0, -15);

				exhaust.FillColor = Colors::Transparent;
				exhaust.OutlineColor = Color(16, 196, 255);
				exhaust.OutlineThickness = 1.5;

				exhaust.Position = Position;
				exhaust.Rotation = Rotation;

				rend.Draw(exhaust);
			}
		}

		bool Accel;
		float Anim;

		float Rotation;
		Vec2 Position;
		Vec2 Inertia;
	}
}

class Asteroids : IGame
{

	void StartNewGame()
	{
		mFinished = false;
	}

	void EndGame()
	{
		mFinished = true;
		mGameShip = Games::Asteroids::Ship();
	}

	void Update(float dt)
	{
		mAnimTime += dt;

		mGameShip.Anim = mAnimTime * 25;
	}

	void Tick(float dt)
	{
		Input@ acc = Inputs::GetInput(0);
		Input@ turn = Inputs::GetInput(1);

		mGameShip.Accel = acc.Pressed;
		if (mGameShip.Accel)
			mGameShip.Inertia += Vec2(cos(mGameShip.Rotation * Math::D2R - Math::HALF_PI),
									  sin(mGameShip.Rotation * Math::D2R - Math::HALF_PI));

		mGameShip.Position += mGameShip.Inertia * dt;
		mGameShip.Rotation += turn.CombinedValue * dt * 90;

		Rect area(-256, -256, 512, 512);
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

/*
		Text gameTitle("Asteroids");
		gameTitle.CharacterSize = 18;
		gameTitle.Origin = Vec2(gameTitle.LocalBounds.Size.X / 2, 0);
		gameTitle.Position = area.TopLeft + Vec2(area.Size.X / 2, 0);
		rend.Draw(gameTitle);
*/

		Games::Asteroids::Ship exampleShip();

		exampleShip.Accel = true;
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

		mGameShip.Draw(rend);

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
		background.OutlineThickness = 64;
		background.OutlineColor = Colors::Black;

		rend.Draw(background);
	}

	bool Finished { get const { return mFinished; } }
	string Name { get const { return "Asteroids"; } }
	int Score { get const { return TIME_AS_SCORE; } }

	private Games::Asteroids::Ship mGameShip;
	private bool mFinished;
	private float mAnimTime;
}

}