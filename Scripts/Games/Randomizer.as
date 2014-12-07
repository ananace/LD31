#include "IGame.as"

#include "Asteroids.as"
#include "AsteroidDodger.as"
#include "DodgeBall.as"
#include "LightCycle.as"

namespace Games
{
	IGame@ CreateGame(string&in name)
	{
		if (name == "Asteroids")
			return Games::Asteroids();
		else if (name == "Asteroid Dodger")
			return Games::AsteroidDodger();
		else if (name == "Light Cycles")
			return Games::LightCycle();
		else if (name == "Dodge 'em")
			return Games::DodgeBall();

		return null;
	}

	IGame@ CreateGame()
	{
		switch(Math::Random(0, 2))
		{
		case 0:
			return Games::Asteroids();
		case 1:
			return Games::AsteroidDodger();
		case 2:
//			return Games::LightCycle();
//		case 3:
			return Games::DodgeBall();
		}

		return null;
	}

	void FromNames(grid<IGame@>& ret, grid<string>@ names)
	{
		ret.resize(names.width(), names.width());

		for (uint x = 0; x < names.width(); ++x)
			for (uint y = 0; y < names.width(); ++y)
				@ret[x, y] = CreateGame(names[x, y]);
	}

	void RandomizeGames(grid<IGame@>& ret, uint size)
	{
		ret.resize(size, size);

		for (uint x = 0; x < size; ++x)
			for (uint y = 0; y < size; ++y)
				@ret[x, y] = CreateGame();
	}
}