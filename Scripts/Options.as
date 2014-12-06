#include "IState.as"

namespace States
{

class Options : IState
{
	void Init(StateMachine@ man)
	{
		@mStateMan = man;
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

		Text bind("Binds:");
		bind.Move(15, 15);
		rend.Draw(bind);

		for (uint i = 0; i < binds.length; ++i)
		{

		}
	}

	string Name { get const { return "Options"; } }

	private StateMachine@ mStateMan;
}

}
