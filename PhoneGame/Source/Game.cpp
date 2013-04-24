#include "Game.h"

#include "MaloWLib/NetworkPacket.h"

Game::Game()
{
	this->networkController = NULL;
	this->gameMode = -1;
	this->go = true;
	this->play = true;
}

Game::~Game()
{
	if(this->networkController)
	{
		delete this->networkController;
		this->networkController = NULL;
	}
}

void Game::NewNetworkClient( MaloW::ClientChannel* cc )
{
	cc->setNotifier(this);
	cc->Start();
	NewNetworkClientEvent* nnce = new NewNetworkClientEvent(cc);
	this->PutEvent(nnce);
}

void Game::Play()
{
	GetGraphics()->GetKeyListener()->SetCursorVisibility(false);
	this->play = true;
	iImage* gameModeImage = GetGraphics()->CreateImage(Vector2(0, 0), Vector2(GetGraphics()->GetEngineParameters().WindowWidth, GetGraphics()->GetEngineParameters().WindowHeight), "Media/ChooseGameMode.png");
	GetGraphics()->StartRendering();
	while(play)
	{	
		float diff = GetGraphics()->Update();
		if(GetGraphics()->GetKeyListener()->IsPressed('1'))
		{
			this->gameMode = 1;
			if(this->networkController)
			{
				this->networkController->cc->sendData("KEYBOARD CHOSE MODE: 1");
			}
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('2'))
		{
			this->gameMode = 2;
			if(this->networkController)
			{
				this->networkController->cc->sendData("KEYBOARD CHOSE MODE: 2");
			}
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('3'))
		{
			this->gameMode = 3;
			if(this->networkController)
			{
				this->networkController->cc->sendData("KEYBOARD CHOSE MODE: 3");
			}
		}

		switch (this->gameMode)
		{
			// Phone has sent -2 which means quit.
		case -2:
			play = false;
			break;

			// -1 means that keep looping like 0 but that it's the first loop so send msg to phone that it should choose mode.
		case -1:
			this->HandleEvent(diff);
			if(this->networkController)
			{
				this->networkController->cc->sendData("CHOOSE GAMEMODE");
				this->gameMode = 0;
			}
			break;

			// 0 is default and means keep looping waiting for input from phone.
		case 0:
			this->HandleEvent(diff);
			break;

			// 1 is play game mode 1.
		case 1:
			gameModeImage->SetOpacity(0.0f);
			this->PlayGameMode1();
			this->gameMode = -1;
			gameModeImage->SetOpacity(1.0f);
			break;

			// 2 is play game mode 2.
		case 2:
			gameModeImage->SetOpacity(0.0f);
			this->PlayGameMode2();
			this->gameMode = -1;
			gameModeImage->SetOpacity(1.0f);
			break;

		case 3:
			gameModeImage->SetOpacity(0.0f);
			this->PlayGameMode4();
			this->gameMode = -1;
			gameModeImage->SetOpacity(1.0f);
			break;

		case 4:
			gameModeImage->SetOpacity(0.0f);
			this->PlayGameMode3();
			this->gameMode = -1;
			gameModeImage->SetOpacity(1.0f);
			break;

		default:
			break;
		}

		if(GetGraphics()->GetKeyListener()->IsPressed('Q'))
		{
			play = false;
		}

		Sleep(2);
	}

	if(this->networkController)
	{
		this->networkController->cc->sendData("EXITING");
	}

	GetGraphics()->DeleteImage(gameModeImage);
}

void Game::HandleEvent(float diff)
{
	// Take care of packets / events
	if(MaloW::ProcessEvent* ev = this->PeekEvent())
	{
		// Network packet containing data
		if(MaloW::NetworkPacket* np = dynamic_cast<MaloW::NetworkPacket*>(ev))
		{
			string msg = np->getMessage();

			// Accelerometer update
			if(msg.substr(0, 3) == "ACC")
			{
				msg = msg.substr(4);
				int splitpos = msg.find(' ');
				string xstr = msg.substr(0, splitpos);
				msg = msg.substr(splitpos + 1);
				splitpos = msg.find(' ');
				string ystr = msg.substr(0, splitpos);
				msg = msg.substr(splitpos + 1);
				string zstr = msg;

				float x = atof(xstr.c_str());
				float y = atof(ystr.c_str());
				float z = atof(zstr.c_str());


				Vector3 dir = Vector3(x, y, z);
				dir.Normalize();
				this->networkController->direction = dir;
			}

			// Speed message
			if(msg.substr(0, 3) == "SPD")
			{
				msg = msg.substr(4);
				float spd = atof(msg.c_str());
				this->networkController->speed = spd;
			}
			// Answer current game mode
			if(msg.substr(0, 7) == "RESTART")
			{
				this->networkController->needRestart = true;
			}
			// Answer current game mode
			if(msg.substr(0, 8) == "GET MODE")
			{
				this->networkController->cc->sendData("CURRENT MODE: " + MaloW::convertNrToString(this->gameMode));
			}

			// Gamemode change
			if(msg.substr(0, 3) == "GAM")
			{
				msg = msg.substr(4);
				this->gameMode = atoi(msg.c_str());
			}


			if(msg.substr(0, 4) == "QUIT")
			{
				this->go = false;
			}

			if(msg.substr(0, 4) == "EXIT")
			{
				this->go = false;
				this->play = false;
			}

			// Ping request
			if(msg == "PING")
				this->networkController->cc->sendData("PING");
		}

		// New client connected
		if(NewNetworkClientEvent* nnce = dynamic_cast<NewNetworkClientEvent*>(ev))
		{
			if(this->networkController)
			{
				MaloW::Debug("Dropping current connection due to a new one incomming.");
				delete this->networkController;
				this->networkController = NULL;
			}
			this->networkController = new NetworkController();
			this->networkController->cc = nnce->GetCC();
		}

		delete ev;
	}
}
