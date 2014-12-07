#include "Player.as"

class Lobby
{
	Lobby(string name)
	{
		Name = name;
	}

	int opCmp(Lobby@ l) { return Name.opCmp(l.Name); }

	void Message(string message)
	{
		ChatMsgs.insertLast(message);

		if (ChatMsgs.length > 6)
			ChatMsgs.removeAt(0);
	}

	Player@ Host;
	array<Player@> Players;
	grid<string> GameNames;
	array<string> ChatMsgs;
	string Name;

	uint8 GridSize;
}
