#include "IState.as"

namespace States
{

class MainMenu : IState
{
	void Update(float dt)
	{
		totalTime += dt;
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

		Text title("Time until LD31 Theme:\n\t\t\t" + (diff.Hour < 10 ? "0" : "") + diff.Hour +
			":" + (diff.Minute < 10 ? "0" : "") + diff.Minute +
			":" + (diff.Second < 10 ? "0" : "") + diff.Second);
		title.Position = rend.View.Size / 2;
		title.Origin = title.LocalBounds.Size / 2;

		title.Move(sin(totalTime / 1.5) * 32, cos(totalTime / 1.5) * 32);
		title.Rotation = sin(totalTime) * Math::PHI;

		uint8 colorVal = uint8(232 + sin(totalTime * 6) * 12);
		Color col = Colors::White;
		col.A = colorVal;
		title.Color = col;

		rend.Draw(title);
	}

	string Name
	{
		get const { return "Main Menu"; }
	}

	private float totalTime;
}

}
