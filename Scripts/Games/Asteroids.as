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
			ship.OutlineThickness = 1.5;

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

	}

	void EndGame()
	{

	}

	void Update(float dt)
	{
		mAnimTime += dt;
	}

	void Tick(float)
	{

	}

	void DrawQuick(Renderer@ rend, Rect&in area)
	{
		Shapes::Rectangle background(area);

		background.FillColor = Colors::Black;
		background.OutlineThickness = 2;
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

	void DrawFull(Renderer@)
	{

	}

	bool Finished { get const { return mFinished; } }
	string Name { get const { return "Asteroids"; } }
	int Score { get const { return TIME_AS_SCORE; } }

	private bool mFinished;
	private float mAnimTime;
}

}