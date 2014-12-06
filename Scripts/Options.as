#include "IState.as"

namespace States
{

class Options : IState
{
	void Init(StateMachine@ man)
	{
		@mStateMan = man;

		Inputs::SetDeadzone(Joystick::Axis::X, 0.25);
		Inputs::SetDeadzone(Joystick::Axis::Y, 0.25);
	}

	void Tick(float)
	{

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

		Text bind("<< Binds:");
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

		bind.CharacterSize = 20;
		bind.Move(32, 36);

		for (uint8 i = 0; i < uint8(binds.length); ++i)
		{
			Input@ inp = Inputs::GetInput(i);

			if (bind.GlobalBounds.Contains(rend.MousePos))
			{
				if (Mouse::IsPressed(Mouse::Button::Left) && !Inputs::Disabled && !Inputs::Binding)
				{
					Inputs::StartBind(i);
					println("Binding " + binds[i] + "...");
				}
				bind.Color = Colors::Yellow;
			}

			bind.String = binds[i];
			rend.Draw(bind);

			bind.Color = Colors::White;

			float dist = bind.LocalBounds.Width * 1.2;
			bind.Move(dist, 0);
			auto bindData = inp.Bind;
			if (bindData.Keyboard)
			{
				bind.String = "- Bound to Key " + bindData.KeyboardKey();

				rend.Draw(bind);
			}
			else
			{
				bind.String = "- Bound to Joystick " + (bindData.JoystickAxis ?
					"Axis " + axisToString(bindData.JoystickAxis()) :
					"Button " + bindData.JoystickButton());

				rend.Draw(bind);

				if (bindData.JoystickAxis)
				{
					float extraDist = bind.LocalBounds.Width;
					bind.Move(extraDist, 0);

					bind.String = ", Deadzone: " + Inputs::GetDeadzone(bindData.JoystickAxis()) + ", Sensitivity: " + Inputs::GetSensitivity(bindData.JoystickAxis());

					rend.Draw(bind);

					dist += extraDist;
				}
			}
			bind.Move(-dist, 0);

			bind.Move(0, bind.LocalBounds.Height + 4);
		}
	}

	private string axisToString(Joystick::Axis a)
	{
		switch(a)
		{
			case Joystick::Axis::X: return "X";
			case Joystick::Axis::Y: return "Y";
			case Joystick::Axis::Z: return "Z";
			case Joystick::Axis::R: return "RY";
			case Joystick::Axis::U: return "RX";
			case Joystick::Axis::V: return "X";
			case Joystick::Axis::PovX: return "Pov X";
			case Joystick::Axis::PovY: return "Pov Y";
		}
		return "<Unknown>";
	}

	string Name { get const { return "Options"; } }

	private StateMachine@ mStateMan;
}

}
