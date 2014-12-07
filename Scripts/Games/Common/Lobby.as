#include "Player.as"

class Lobby
{
	Lobby(string name)
	{
		Name = name;
	}

	int opCmp(Lobby@ l) { return Name.opCmp(l.Name); }

	Player@ Host;
	array<Player@> Players;
	grid<string> GameNames;
	string Name;

	uint8 GridSize;
}
