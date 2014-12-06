#include "IState.as"
#include "MainMenu.as"

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
		FrameTime += dt;
		if (FrameTime >= 1)
		{
			FrameTime -= 1;
			FPS = Frames;
			Frames = 0;
		}

		IState@ state = @CurState();
		if (state is null)
			return;

		state.Update(dt);
	}

	[Hook::BindHook("Draw")]
	void Draw(Renderer@ rend)
	{
		++Frames;

		IState@ state = @CurState();
		if (state is null)
			return;

		state.Draw(rend);
	}

	[Hook::BindHook("DrawUi")]
	void DrawUi(Renderer@ rend)
	{
		IState@ state = @CurState();
		if (state !is null)
			state.DrawUi(rend);

		Text fpsCounter("FPS: " + FPS);
		fpsCounter.CharacterSize = 16;
		fpsCounter.Move(5, 2);
		rend.Draw(fpsCounter);
	}

	void PushState(IState@ state)
	{
		println("[StateMachine] Pushing " + state.Name + " state.");

		StateArray.insertLast(state);
	}

	IState@ PopState()
	{
		IState@ state = @StateArray[StateArray.length - 1];
		StateArray.removeLast();

		println("[StateMachine] Popped " + state.Name + " state.");
		return state;
	}

	IState@ CurState()
	{
		if (StateArray.isEmpty())
			return null;

		return StateArray[StateArray.length - 1];
	}

	private array<IState@> StateArray;
	private float FrameTime;
	private int Frames, FPS;
}
