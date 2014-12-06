#include "Player.as"

class HighscoreEntry
{
	Player@ Player;
	int Score;

	int opCmp(HighscoreEntry&in rhs)
	{
		return (Score < rhs.Score ? -1 : (Score == rhs.Score ? 0 : 1));
	}
}

class Highscore
{
	void PushScore(Player@ p, int score)
	{
		HighscoreEntry entry;

		@entry.Player = p;
		entry.Score = score;

		mEntries.insertLast(entry);
	}
	void Sort(int dir)
	{
		if (dir > 0)
			mEntries.sortDesc();
		else
			mEntries.sortAsc();
	}

	Player@ Leader { get const { return mEntries[0].Player; } }
	array<HighscoreEntry> Scores { get const { return mEntries; } }

	private array<HighscoreEntry> mEntries;
}