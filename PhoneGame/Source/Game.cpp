#include "Game.h"

#include "MaloWLib/NetworkPacket.h"

Game::Game()
{
	this->networkController = NULL;
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

void Game::Play(int mode)
{
	switch (mode)
	{
	case 1:
		this->PlayGameMode1();
	  break;
	case 2:
		this->PlayGameMode2();
		break;
	default:
		break;
	}
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
