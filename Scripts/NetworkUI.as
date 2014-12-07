#include "IState.as"
#include "Games/Common/Player.as"

namespace States
{

class NetworkUI : IState
{
	void Init(StateMachine@ stateman)
	{
		@mStateMan = @stateman;

		mTempColor = Colors::Black;
	}

	void Update(float dt)
	{
		mAnim += dt;
	}

	void Tick(float)
	{
		if (Inputs::ReadingText)
		{
			mTempName = Inputs::Text;
		}
		else if (mNameInput)
		{
			mNameInput = false;
		}

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

			if (mNameInput)
				inputBox.OutlineColor = Colors::Green;

			rend.Draw(inputBox);

			Text boxText(mTempName);
			if (mNameInput)
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

				if (Mouse::IsPressed(Mouse::Button::Left) && !mLastClick)
				{
					@mLocalPlayer = Player(mTempName, mTempColor);

					Packet playerInfo;

					playerInfo << uint8(0) << uint16(0xbeef) << mTempColor.R << mTempColor.G << mTempColor.B << mTempName;

					if (!Network::SendPacket(playerInfo))
						@mLocalPlayer = null;
				}
			}

			rend.Draw(title);
		}
		else
		{
			Text title("Network games");

			if (title.GlobalBounds.Contains(rend.MousePos))
			{
				title.Color = Colors::Yellow;

				if (Mouse::IsPressed(Mouse::Button::Left) && !mLastClick)
				{
					println("Packet");

					Packet updateList;
					updateList << uint8(1) << uint8(0xBA) << "Test";

					Network::SendPacket(updateList);
				}
			}

			rend.Draw(title);
		}

		mLastClick = Mouse::IsPressed(Mouse::Button::Left);
	}

	void Packet(Packet&in p)
	{
		println("Got packet");

		uint8 chan = 0;

		p >> chan;

		switch(chan)
		{
		case 1: {
				uint8 cmd = 0;
				uint16 result = 0;

				p >> cmd >> result;

				println("Got result " + result + " from command " + cmd);
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

	private bool mNameInput, mLastClick;

	private string mTempName;
	private Color mTempColor;
	private float mAnim;

	private array<string> mLobbyList;

	private StateMachine@ mStateMan;
	private Player@ mLocalPlayer;
}

}
