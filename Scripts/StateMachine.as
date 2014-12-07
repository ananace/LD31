#include "IState.as"
#include "MainMenu.as"

[ObjectType("StateMachine")]
class StateMachine
{
	StateMachine()
	{
		DrawFPS = false;
	}

	[Hook::BindHook("Tick")]
	void Tick(float dt)
	{
		bool pressed = Keyboard::IsPressed(Keyboard::Key::F3);
		if (pressed && !LastPressed)
			DrawFPS = !DrawFPS;
		LastPressed = pressed;

		if (CurState() is null)
		{
			StateArray.length = 0;
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

	[Hook::BindHook("Packet")]
	void Packet(Packet&in p)
	{
		IState@ state = @CurState();
		if (state is null)
			return;

		state.Packet(p);
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

		if (!DrawFPS)
			return;

		Text fpsCounter("FPS: " + FPS);
		fpsCounter.CharacterSize = 16;
		fpsCounter.Origin = Vec2(fpsCounter.LocalBounds.Size.X, 0);
		fpsCounter.Position = Vec2(rend.View.Size.X, 0);
		fpsCounter.Move(-5, -2);
		rend.Draw(fpsCounter);
	}

	void PushState(IState@ state)
	{
		println("[StateMachine] Pushing " + state.Name + " state...");

		state.Init(this);
		StateArray.insertLast(state);
	}

	IState@ PopState()
	{
		IState@ state = @StateArray[StateArray.length - 1];
		StateArray.removeLast();

		println("[StateMachine] Popped " + state.Name + " state...");
		return state;
	}

	IState@ CurState()
	{
		if (StateArray.isEmpty())
			return null;

		return StateArray[StateArray.length - 1];
	}

	private bool DrawFPS, LastPressed;

	private array<IState@> StateArray;
	private float FrameTime;
	private int Frames, FPS;
}
