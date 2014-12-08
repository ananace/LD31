#include "IState.as"
#include "Games/Common/Player.as"

namespace States
{

class WinScreen : IState
{
	WinScreen(Player@ winner, grid<Games::IGame@>@ gameGrid = null)
	{
		@mWinner = winner;
		//@mGameGrid = gameGrid;
	}

	void Init(StateMachine@ man)
	{
		@mStateMan = man;
	}
	void Packet(Packet&in){}

	void Tick(float)
	{

	}
	void Update(float dt)
	{
		mAnimTime += dt;
	}
	void Draw(Renderer@ rend)
	{

	}
	void DrawUi(Renderer@ rend)
	{
		Text winnerName(mWinner.Name);

		winnerName.CharacterSize = 40;

		winnerName.Origin = Vec2(winnerName.LocalBounds.Width / 2, 0);
		winnerName.Position = Vec2(rend.View.Size.X / 2 + cos(mAnimTime / 2) * 10, 32 + sin(mAnimTime) * 10);
		winnerName.Rotation = sin(mAnimTime) * Math::DOUBLE_PI;

		rend.Draw(winnerName);

		winnerName.CharacterSize = 20;
		winnerName.Move(0, 45);

		winnerName.String = "Takes home the victory";
		winnerName.Origin = Vec2(winnerName.LocalBounds.Width / 2, 0);

		rend.Draw(winnerName);

		Text rematch("[ I demand a rematch! ]");
		rematch.Position = rend.View.Center;
		rematch.Origin = rematch.LocalBounds.Size / 2;

		if (rematch.GlobalBounds.Contains(rend.MousePos))
		{
			rematch.Color = Colors::Yellow;
			rematch.SetScale(1.2);

			if (Mouse::IsPressed(Mouse::Button::Left))
				mStateMan.PopState();
		}

		rend.Draw(rematch);
	}

	string Name { get const { return "WinScreen"; } }

	private float mAnimTime;
	private StateMachine@ mStateMan;
	private Player@ mWinner;
	//private grid<Games::IGame@>@ mGameGrid;
}

}
