#include "States/IState.as"
#include "States/MainMenu.as"

[ObjectType("StateMachine")]
class StateMachine
{
	StateMachine()
	{
	}

	[Hook::BindHook("Tick")]
	void Tick(float dt)
	{
		if (CurState() is null)
		{
			PushState(States::MainMenu());
		}

		CurState().Tick(dt);
	}

	[Hook::BindHook("Update")]
	void Update(float dt)
	{
		IState@ state = @CurState();
		if (state is null)
			return;

		state.Update(dt);
	}

	[Hook::BindHook("Draw")]
	void Draw(Renderer@ rend)
	{
		IState@ state = @CurState();
		if (state is null)
			return;

		state.Draw(rend);
	}

	[Hook::BindHook("DrawUi")]
	void DrawUi(Renderer@ rend)
	{
		IState@ state = @CurState();
		if (state is null)
			return;

		state.DrawUi(rend);
	}

	void PushState(IState@ State)
	{
		StateArray.insertLast(State);
	}

	IState@ PopState()
	{
		IState@ State = @StateArray[StateArray.length - 1];
		StateArray.removeLast();
		return State;
	}

	IState@ CurState()
	{
		if (StateArray.isEmpty())
			return null;

		return StateArray[StateArray.length - 1];
	}

	private array<IState@> StateArray;
}
