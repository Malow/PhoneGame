#include "Game.h"

#include "MaloWLib/NetworkPacket.h"

Game::Game()
{
	this->networkController = NULL;
	this->gameMode = -1;
	this->go = true;
	this->play = true;
	this->pingTimer = 0.0f;
	this->isPinging = false;
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
	bool pingb = true;

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

			// 0 is default and means keep looping waiting for input from phone. Also take "P" keypress to make a ping and write MS in MaloW::Debug.
		case 0:
			if(isPinging)
				this->pingTimer += diff;
			if(GetGraphics()->GetKeyListener()->IsPressed('P'))
			{
				if(pingb)
				{
					if(this->networkController)
					{
						this->networkController->cc->sendData("PING");
						this->isPinging = true;
						this->pingTimer = 0.0f;
					}
					pingb = false;
				}			
			}
			else
				pingb = true;

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

			// Aim message for GameMode4 (3)
			if(msg.substr(0, 3) == "AIM")
			{
				msg = msg.substr(4);
				int spacePos = msg.find(' ');
				float x = atof(msg.substr(0, spacePos).c_str());
				float y = atof(msg.substr(spacePos + 1).c_str());
				this->networkController->aim = Vector2(x, y);
			}

			// Speed message
			if(msg.substr(0, 3) == "SPD")
			{
				msg = msg.substr(4);
				float spd = atof(msg.c_str());
				this->networkController->speed = spd;
			}

			// Shoot message for GameMode4 (3)
			if(msg.substr(0, 5) == "SHOOT")
			{
				this->networkController->shoot = true;
			}

			// Restart for gameMode2
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

			// Quit the game mode
			if(msg.substr(0, 4) == "QUIT")
			{
				this->go = false;
			}

			// Exit the program
			if(msg.substr(0, 4) == "EXIT")
			{
				this->go = false;
				this->play = false;
			}

			// Ping request
			if(msg == "PING")
			{
				this->isPinging = false;
				MaloW::Debug("Ping done: " + MaloW::convertNrToString(this->pingTimer) + " ms.");
				this->pingTimer = 0.0f;
			}
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

void Game::FinishScreen( int score, string gamemode, float time )
{
	MaloW::Debug("Game " + gamemode + " Finished after " + MaloW::convertNrToString(time) + " seconds. Score: " + MaloW::convertNrToString(score));
	iText* finishText = GetGraphics()->CreateText(string("Finished after " + MaloW::convertNrToString((int)time) + " seconds! Score: " + MaloW::convertNrToString(score)).c_str(), 
		Vector2((GetGraphics()->GetEngineParameters().WindowWidth / 2) * 0.5f, GetGraphics()->GetEngineParameters().WindowHeight / 2), 2.0f, "Media/fonts/newBorder");
	while(GetGraphics()->IsRunning() && go)
	{
		Sleep(1);
		// Updates GFX
		float diff = GetGraphics()->Update();

		// Handle events such as network packets and client connections
		this->HandleEvent(diff);

		if(GetGraphics()->GetKeyListener()->IsPressed(VK_ESCAPE))
			go = false;
	}

	GetGraphics()->DeleteText(finishText);
}
