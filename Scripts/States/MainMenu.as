#include "IState.as"

namespace States
{

class MainMenu : IState
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

	void Tick(float)
	{

	}

	void Draw(Renderer@)
	{

	}

	void DrawUi(Renderer@ rend)
	{
		DateTime target(2014,11,05, 03,00,00);
		DateTime now = DateTime::Now;

		int secDiff = (target.Hour - now.Hour) * 3600 +
		              (target.Minute - now.Minute) * 60 +
		              (target.Second - now.Second);

		DateTime diff(now.Year,now.Month,now.Day, 0,0, secDiff);

		Frames++;

		Text fpsCounter("FPS: " + FPS);
		fpsCounter.CharacterSize = 16;
		fpsCounter.Move(5, 2);
		rend.Draw(fpsCounter);

		Text title("Time until LD31 Theme:\n\t\t\t" + (diff.Hour < 10 ? "0" : "") + diff.Hour +
			":" + (diff.Minute < 10 ? "0" : "") + diff.Minute +
			":" + (diff.Second < 10 ? "0" : "") + diff.Second);
		title.Position = Vec2(rend.View.Size.X / 2, 48);
		title.Origin = title.LocalBounds.Size / 2;

		title.Move(sin(totalTime) * 6, cos(totalTime) * 6);
		title.Rotation = sin(totalTime);

		uint8 colorVal = uint8(232 + sin(totalTime * 4) * 16);
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
