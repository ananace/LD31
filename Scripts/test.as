#include "Menu/MainMenuPage.as"

[ObjectType("Menu")]
class MenuClass
{
	MenuClass()
	{

	}

	[Hook::BindHook("Tick")]
	void Update(float dt)
	{
		if (CurPage() is null)
		{
			PushPage(Menu::MainMenuPage());
		}

		CurPage().Update(dt);
	}

	[Hook::BindHook("DrawUi")]
	void Draw(Renderer@ rend)
	{

		for (uint i = 0; i < PageArray.length; ++i)
		{
			Menu::IPage@ page = @PageArray[i];

			page.Draw(rend);
		}
	}

	void PushPage(Menu::IPage@ page)
	{
		PageArray.insertLast(page);
	}
	Menu::IPage@ PopPage()
	{
		auto page = PageArray[PageArray.length - 1];
		PageArray.removeLast();
		return page;
	}

	Menu::IPage@ CurPage()
	{
		if (PageArray.isEmpty())
			return null;

		return PageArray[PageArray.length - 1];
	}

	private array<Menu::IPage@> PageArray;
}
