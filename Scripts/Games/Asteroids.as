#include "IGame.as"

namespace Games
{

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

		rend.draw(background);

		Shapes::Polygon ship = {
			Vec2(0, -10),
			Vec2(10, 5),
			Vec2(-10, 5)
		};

		ship.FillColor = Colors::Transparent;
		ship.OutlineColor = Colors::White;
		ship.OutlineThickness = 1;

		ship.Position = area.Center;
		ship.Rotation = mAnimTime * 90; // Degrees per second

		rend.Draw(ship);
	}

	void DrawFull(Renderer@)
	{

	}

	bool Finished { get const { return mFinished; } }
	string Name { get const { return "Asteroids"; } }
	int Score { get const { return TIME_AS_SCORE; } }

	private float mAnimTime;
}

}