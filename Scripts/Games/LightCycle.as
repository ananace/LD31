#include "IGame.as"

namespace Games
{
	
class LightCycle : IGame
{
	LightCycle()
	{
		@mHighscore = Highscore();
	}

	void StartNewGame()
	{
		mFinished = false;
	}
	void EndGame()
	{
		mFinished = true;
	}
	void Cleanup()
	{

	}

	void Update(float dt)
	{
		mAnimTimer += dt;
	}
	void Tick(float)
	{

	}

	void DrawQuick(Renderer@ rend, Rect&in area)
	{
		
	}
	void DrawFull(Renderer@ rend, Rect&in area)
	{

	}

	bool Finished { get const { return mFinished; } }
	string Name { get const { return "Light Cycles"; } }
	int Score { get const { return TIME_AS_SCORE; } }
	int ScoreOrder { get const { return SCORE_HIGHEST; } }
	Player@ Owner { get const { return mOwner; } set { @mOwner = value; } }
	Highscore@ Highscore { get const { return mHighscore; } }

	private Highscore@ mHighscore;
	private float mAnimTimer;

	private bool mFinished;
	private Player@ mOwner;
}

}