namespace Games
{

const int TIME_AS_SCORE = -1;

interface IGame
{
	// Game is focused, start a new round
	void StartNewGame();
	// Reset game to unfocused state
	void EndGame();

	// Update anything framerate dependant, runs aways
	void Update(float dt);
	// Fixed tick update loop
	void Tick(float);

	// Draw the game into a tile
	void DrawQuick(Renderer@, Rect&in);
	// Draw the game fully
	void DrawFull(Renderer@);

	// Is this round finished
	bool get_Finished() const;
	// The name of the game
	string get_Name() const;
	// The current player score
	// Can also be the special value TIME_AS_SCORE
	int get_Score() const;
}

}
