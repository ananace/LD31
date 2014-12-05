namespace Menu
{

interface IPage
{
	string get_Name() const;

	void Update(float);
	void Draw(Renderer@);
}

}
