class Player
{
	Player(string name, Color color)
	{
		Name = name;
		Color = color;
	}

	int opCmp(Player@ r) { return Name.opCmp(r.Name); }

	string Name;
	Color Color;
}
