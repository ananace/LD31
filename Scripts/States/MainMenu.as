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

		float fm1 = 1, fm2 = 1, fm3 = 1;

		if (now > target)
		{
			fm1 = 1.5;
			fm2 = 5;
			fm3 = Math::DOUBLE_PI;

			title.String = "Theme is out!!!\n  GO GO GO!";
		}

		title.Position = rend.View.Size / 2;
		title.Origin = title.LocalBounds.Size / 2;

		title.Move(sin(fm1 * totalTime / 1.5) * 32, cos(fm2 * totalTime / 1.5) * 32);
		title.Rotation = sin(totalTime) * Math::PHI * fm3*fm3;

		uint8 colorVal = uint8(200 + sin(totalTime * 6) * 12);
		Color col = Colors::White;
		col.A = colorVal;
		title.Color = col;

		if (fm3 > 1)
		{
			title.SetScale(1 + sin(totalTime) / 3);
		}

		rend.Draw(title);
	}

	string Name
	{
		get const { return "Main Menu"; }
	}

	private float totalTime;
}

}
