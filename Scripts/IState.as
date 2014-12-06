#include "StateMachine.as"

interface IState
{
	string get_Name() const;
	void Init(StateMachine@);

	void Tick(float);
	void Update(float);
	void Draw(Renderer@);
	void DrawUi(Renderer@);
}
