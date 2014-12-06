#include "IState.as"

namespace States
{

class Options : IState
{
	void Init(StateMachine@ man)
	{
		@mStateMan = man;
		mSelected = -1;
	}

	void Tick(float)
	{
		Input@ upDown = Inputs::GetInput(0);
		Input@ action = Inputs::GetInput(2);

		int delta = upDown.CombinedPress;
		if (delta != mOldDelta)
		{
			mSelected += delta;

			mOldDelta = delta;
		}

		if (action.Pressed && !mLastPressed)
		{
			println("Rebind input " + mSelected);
			Inputs::StartBind(mSelected);
		}

		mLastPressed = action.Pressed;
	}
	void Update(float)
	{

	}
	void Draw(Renderer@)
	{

	}
	void DrawUi(Renderer@ rend)
	{
		const array<string> binds = {
			"Up/Down", "Left/Right", "Action"
		};

		Text bind("Binds:");
		bind.Move(15, 15);

		if (bind.GlobalBounds.Contains(rend.MousePos))
		{
			if (Mouse::IsPressed(Mouse::Button::Left) && !Inputs::Disabled)
			{
				mStateMan.PopState();
				return;
			}
			bind.Color = Colors::Yellow;
		}

		rend.Draw(bind);

		bind.Color = Colors::White;

		bind.CharacterSize = 18;
		bind.Move(32, 32);

		for (uint8 i = 0; i < uint8(binds.length); ++i)
		{
			Input@ inp = Inputs::GetInput(i);

			bind.String = (uint8(mSelected) == i ? "> " : "   ") + binds[i] + " - " + inp.CombinedValue;
			rend.Draw(bind);

			bind.Move(0, bind.LocalBounds.Height);
		}
	}

	string Name { get const { return "Options"; } }

	private StateMachine@ mStateMan;
	private int mSelected, mOldDelta;
	private bool mLastPressed;
}

}
