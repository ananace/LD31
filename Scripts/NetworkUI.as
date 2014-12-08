#include "IState.as"
#include "Games/Common/Lobby.as"
#include "Games/Common/Player.as"
#include "Games/Randomizer.as"

namespace States
{

class NetworkUI : IState
{
	void Init(StateMachine@ stateman)
	{
		@mStateMan = @stateman;

		mTempColor = random();
	}

	void Update(float dt)
	{
		mAnim += dt;
	}

	void Tick(float)
	{
		if (mLocalPlayer is null)
		{
			if (Inputs::ReadingText)
			{
				mTempName = Inputs::Text;

				if (Keyboard::IsPressed(Keyboard::Key::Return))
					Inputs::StopText();
			}
			else if (mNameInput)
			{
				mNameInput = false;
			}
		}
		else if (mCurLobby !is null)
		{
			if (Keyboard::IsPressed(Keyboard::Key::Return) && !Inputs::Disabled && !Inputs::ReadingText && !mWasReading)
			{
				println("Starting read");
				Inputs::StartText();
			}
			else if (Keyboard::IsPressed(Keyboard::Key::Return) && Inputs::ReadingText && !mWasReading)
			{
				println("Sending message " + Inputs::Text);
				chatMessage(Inputs::Text);
				Inputs::StopText();
			}
		}

		mWasReading = Keyboard::IsPressed(Keyboard::Key::Return);
	}

	void Draw(Renderer@)
	{

	}

	void DrawUi(Renderer@ rend)
	{
		if (mLocalPlayer is null)
		{
			Text title("Choose a name:");

			rend.Draw(title);

			Shapes::Rectangle inputBox(Vec2(256, 32));
			inputBox.Position = title.Position + Vec2(320, 8);
			
			inputBox.FillColor = Colors::Black;
			inputBox.OutlineColor = Colors::White;
			inputBox.OutlineThickness = 1.f;

			if (inputBox.GlobalBounds.Contains(rend.MousePos))
			{
				inputBox.OutlineColor = Colors::Yellow;

				if (Mouse::IsPressed(Mouse::Button::Left) && !mNameInput && !mLastClick)
				{
					mNameInput = true;
					Inputs::StartText();
				}
			}
			else if (Mouse::IsPressed(Mouse::Button::Left) && mNameInput && !mLastClick)
			{
				Inputs::StopText();
			}

			if (Inputs::ReadingText)
				inputBox.OutlineColor = Colors::Green;
			else if (mTempName.isEmpty())
				inputBox.OutlineColor = Colors::Red;

			rend.Draw(inputBox);

			Text boxText(mTempName);
			if (Inputs::ReadingText)
				boxText.String = mTempName + (int(mAnim * 2) % 2 == 0 ? "|" : "");

			boxText.CharacterSize = 18;
			boxText.Position = inputBox.Position;
			boxText.Origin = Vec2(-4, -4);

			rend.Draw(boxText);

			title.Move(0, 48);
			title.String = "Choose your color:";

			rend.Draw(title);

			Shapes::Rectangle colBox(Vec2(32, 32));

			colBox.Position = inputBox.Position + Vec2(0, 44);
			colBox.FillColor = mTempColor;
			colBox.OutlineColor = Colors::White;
			colBox.OutlineThickness = 1.f;

			if (colBox.GlobalBounds.Contains(rend.MousePos))
			{
				colBox.OutlineColor = Colors::Yellow;

				if (Mouse::IsPressed(Mouse::Button::Left) && !mLastClick)
				{
					mTempColor = random();
				}
			}

			rend.Draw(colBox);

			title.Move(32, 96);
			title.String = "Continue >>";

			if (title.GlobalBounds.Contains(rend.MousePos))
			{
				title.Color = Colors::Yellow;

				if (Mouse::IsPressed(Mouse::Button::Left) && !mLastClick && !mTempName.isEmpty())
				{
					@mLocalPlayer = Player(mTempName, mTempColor);

					Packet playerInfo;

					playerInfo << uint8(0) << uint16(0xbeef) << mTempColor.R << mTempColor.G << mTempColor.B << mTempName;

					if (!Network::SendPacket(playerInfo))
					{
						@mLocalPlayer = null;
					}
					else
						updateLobbies();
				}
			}

			rend.Draw(title);
		}
		else if (mCurLobby is null)
		{
			Text title("Network games:");
			title.Position = Vec2(16, 32);

			rend.Draw(title);

			title.CharacterSize = 20;

			title.Move(242, 8);
			title.String = "[ Update ]";

			if (title.GlobalBounds.Contains(rend.MousePos))
			{
				title.Color = Colors::Yellow;
				if (Mouse::IsPressed(Mouse::Button::Left) && !mLastClick)
				{
					updateLobbies();
				}
			}

			rend.Draw(title);
			title.Color = Colors::White;

			title.Move(96, 0);
			title.String = "[ Create Lobby ]";

			if (title.GlobalBounds.Contains(rend.MousePos))
			{
				title.Color = Colors::Yellow;
				if (Mouse::IsPressed(Mouse::Button::Left) && !mLastClick)
				{
					createLobby(mLocalPlayer.Name + "'s Lobby");
				}
			}

			rend.Draw(title);
			title.Color = Colors::White;

			

			title.Move(16, 32);

			for (uint i = 0; i < mLobbyList.length; ++i)
			{
				title.String = mLobbyList[i].Name;

				if (title.GlobalBounds.Contains(rend.MousePos))
				{
					title.Color = Colors::Yellow;

					if (Mouse::IsPressed(Mouse::Button::Left) && !mLastClick)
					{
						joinLobby(@mLobbyList[i]);
					}
				}

				rend.Draw(title);
				title.Move(0, 36);
				title.Color = Colors::White; 
			}
		}
		else
		{
			Text playList(mCurLobby.Name);
			playList.Position = Vec2(16, 32);

			rend.Draw(playList);

			playList.String = "Players:";

			playList.CharacterSize = 20;
			playList.Move(8, 32);

			rend.Draw(playList);
			
			playList.Move(24, 6);
			Shapes::Rectangle rekt(Rect(8, 38, 16.f, 16.f));
			rekt.Origin = Vec2(-4, -4);
			rekt.Position = Vec2(16, 32);
			rekt.Move(8, 37);

			rekt.OutlineColor = Colors::White;
			rekt.OutlineThickness = 1;

			for (uint i = 0; i < mCurLobby.Players.length; ++i)
			{
				rekt.Move(0, 26);
				playList.Move(0, 26);
				Player@ p = mCurLobby.Players[i];

				rekt.FillColor = p.Color;
				rend.Draw(rekt);

				playList.String = p.Name;

				rend.Draw(playList);
			}

			if (@mCurLobby.Players[0] == @mLocalPlayer)
			{
				Text launch("<Launch the game>");

				launch.Origin = Vec2(launch.LocalBounds.Width / 2, 0);
				launch.Position = Vec2(rend.View.Size.X / 2, 0);

				if (launch.GlobalBounds.Contains(rend.MousePos))
				{
					launch.Color = Colors::Yellow;

					if (Mouse::IsPressed(Mouse::Button::Left) && !mLastClick)
					{
						launchGame();
					}
				}

				rend.Draw(launch);
			}

			Text chat;
			chat.CharacterSize = 16;
			chat.Position = Vec2(0, rend.View.Size.Y);
			chat.Origin = Vec2(0, 22);

			if (Inputs::ReadingText)
			{
				chat.String = "> " + Inputs::Text + (int(mAnim) % 2 == 0 ? "|" : "");
				rend.Draw(chat);
				chat.Move(0, -26);
			}

			for (uint i = 0; i < mCurLobby.ChatMsgs.length; ++i)
			{
				chat.String = mCurLobby.ChatMsgs[mCurLobby.ChatMsgs.length-i-1];
				
				rend.Draw(chat);

				chat.Move(0, -22);
			}
		}

		Text back("<< Back");
		back.CharacterSize = 18;

		if (back.GlobalBounds.Contains(rend.MousePos))
		{
			back.Color = Colors::Yellow;

			if (Mouse::IsPressed(Mouse::Button::Left) && !mLastClick)
			{
				Network::Disconnect();
				mStateMan.PopState();
			}
		}

		rend.Draw(back);

		mLastClick = Mouse::IsPressed(Mouse::Button::Left) || Mouse::IsPressed(Mouse::Button::Right);
	}

	void createLobby(string name)
	{
		Packet create;
		create << uint8(1) << uint8(0xBA) << name;

		Network::SendPacket(create);
	}

	void joinLobby(Lobby@ lobby, string password = "")
	{
		Packet join;
		join << uint8(1) << uint8(0xBB) << lobby.Name << password;

		Network::SendPacket(join);
	}

	void launchGame()
	{
		if (@mLocalPlayer == @mCurLobby.Players[0])
		{
			grid<Games::IGame@> temp;
			Games::RandomizeGames(temp, mCurLobby.GridSize);

			mCurLobby.GameNames.resize(mCurLobby.GridSize, mCurLobby.GridSize);
			for (uint x = 0; x < mCurLobby.GridSize; ++x)
				for (uint y = 0; y < mCurLobby.GridSize; ++y)
					mCurLobby.GameNames[x, y] = temp[x, y].Name;
		}

		Packet games;
		games << uint8(1) << uint8(0x6A) << mCurLobby.GridSize;
		for (uint x = 0; x < mCurLobby.GridSize; ++x)
			for (uint y = 0; y < mCurLobby.GridSize; ++y)
				games << mCurLobby.GameNames[x, y];

		Network::SendPacket(games);
	}

	void updateLobbies()
	{
		Packet create;
		create << uint8(1) << uint8(0xDE);

		Network::SendPacket(create);

		mLobbyList.length = 0;
	}

	void chatMessage(string msg)
	{
		if (msg.isEmpty())
			return;

		Packet message;
		message << uint8(1) << uint8(0xCE) << msg;

		Network::SendPacket(message);
	}

	void Packet(Packet&in p)
	{
		uint8 chan = 0;

		p >> chan;

		switch(chan)
		{
		case 0: { // Lobby list
				uint8 cmd = 0;
				p >> cmd;

				switch (cmd)
				{
				case 0xBA: {
						uint8 reason = 0;
						p >> reason;

						print("Failed to enter the lobby");

						switch (reason)
						{
						case 0xAE: println(", couldn't create due to name conflict"); break;
						case 0xA5: println(", password incorrect"); break;
						case 0xFF: println(", lobby full"); break;
						default: println(", unknown reason"); break;
						}
					} break;

				case 0xCA: {
						string name;
						p >> name;

						println("Now in the lobby '" + name + "'");

						int i = mLobbyList.find(Lobby(name));
						if (i < 0)
						{
							updateLobbies();

							@mCurLobby = Lobby(name);
							mCurLobby.Players.insertLast(mLocalPlayer);

							mLobbyList.insertLast(mCurLobby);
							mLobbyList.sortAsc();
						}
						else
							@mCurLobby = mLobbyList[i];
					} break;

				case 0xDE: {
						uint8 size = 0;
						string name;
						p >> size >> name;

						println("Lobby available: '" + name + "'");

						int i = mLobbyList.find(Lobby(name));
						if (i >= 0)
						{
							mLobbyList[i].GridSize = size;
						}
						else
						{
							Lobby@ lobby = Lobby(name);
							lobby.GridSize = size;
							mLobbyList.insertLast(lobby);
							mLobbyList.sortAsc();
						}
					} break;
				}
			} break;

		case 1: { // In-Lobby
				uint8 act = 0;
				p >> act;

				switch (act)
				{
				case 0xAD: {
						string name;
						p >> name;

						println(name + " left the lobby");

						int i = mCurLobby.Players.find(Player(name, Colors::Transparent));
						if (i >= 0)
							mCurLobby.Players.removeAt(i);
					} break;

				case 0xCA: {
						string name;
						Color col;
						p >> col.R >> col.G >> col.B >> name;

						println(name + " entered the lobby");

						mCurLobby.Players.insertLast(Player(name, col));
					} break;

				case 0XCE: {
						string msg;
						p >> msg;

						println(msg);
						mCurLobby.Message(msg);
					} break;

				case 0x6A: {
						uint8 size = 0;
						p >> size;
						println("Size: " + size);

						mCurLobby.GameNames.resize(size, size);
						for (uint8 x = 0; x < size; ++x)
							for (uint8 y = 0; y < size; ++y)
							{
								p >> mCurLobby.GameNames[x, y];
								println("[" + x + "," + y + "] is " + mCurLobby.GameNames[x,y]);
							}

						println("Launching game");

						States::GameState@ game = States::GameState(mCurLobby);
						@game.LocalPlayer = mLocalPlayer;

						mStateMan.PopState();
						mStateMan.PushState(game);
					} break;
				}
			} break;

		case 2: { // In-Game

			} break;
		}
	}

	private Color random()
	{
		Color ret;

		ret.R = Math::Random(0, 255);
		ret.G = Math::Random(0, 255);
		ret.B = Math::Random(0, 255);

		return ret;
	}

	string Name
	{
		get const { return "NetworkUI"; }
	}

	private bool mNameInput, mLastClick, mWasReading;

	private string mTempName;
	private Color mTempColor;
	private float mAnim;

	private array<Lobby@> mLobbyList;

	private Lobby@ mCurLobby;
	private StateMachine@ mStateMan;
	private Player@ mLocalPlayer;
}

}
