#include "IGame.as"

namespace Games
{
	
class LightCycle : IGame
{
	void StartNewGame()
	{
		mFinished = false;
	}
	void EndGame()
	{
		mFinished = true;
	}
	void Cleanup()
	{

	}

	void Update(float dt)
	{
		mAnimTimer += dt;
	}
	void Tick(float)
	{

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

		// Draw example

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

	}

	bool Finished { get const { return mFinished; } }
	string Name { get const { return "Light Cycles"; } }
	int Score { get const { return TIME_AS_SCORE; } }
	int ScoreOrder { get const { return SCORE_HIGHEST; } }
	Player@ Owner { get const { return mOwner; } set { @mOwner = value; } }

	private float mAnimTimer;

	private bool mFinished;
	private Player@ mOwner;
}

}