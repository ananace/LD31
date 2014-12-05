#include "Page.as"

namespace Menu
{

class MainMenuPage : IPage
{
	void Update(float dt)
	{
		time += dt;
		totalTime += dt;

		if (time >= 1)
		{
			FPS = Frames;
			Frames = 0;
			
			time -= 1;
		}
	}

	void Draw(Renderer@ rend)
	{
		Frames++;

		Text fpsCounter("FPS: " + FPS);
		fpsCounter.CharacterSize = 16;
		fpsCounter.Move(5, 2);
		rend.Draw(fpsCounter);

		Text title("<Title Screen Goes Here>");
		title.Position = Vec2(rend.View.Size.X / 2, 32);
		title.Origin = title.LocalBounds.Size / 2;

		title.Move(sin(totalTime) * 6, cos(totalTime) * 6);
		title.Rotation = sin(totalTime);

		uint8 colorVal = uint8(200 + sin(totalTime * 4) * 32);
		Color col = Colors::White;
		col.A = colorVal;
		title.Color = col;

		rend.Draw(title);
	}

	string Name
	{
		get const { return "Main Menu"; }
	}

	private float totalTime, time;
	private int Frames, FPS;
}

}
