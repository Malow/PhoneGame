#include "Game.h"

#include "MaloWLib/NetworkPacket.h"

Game::Game()
{
	this->cc = NULL;
}

Game::~Game()
{
	if(this->cc)
	{
		this->cc->Close();
		delete this->cc;
		this->cc = NULL;
	}
}

void Game::NewNetworkClient( MaloW::ClientChannel* cc )
{
	cc->setNotifier(this);
	cc->Start();
	NewNetworkClientEvent* nnce = new NewNetworkClientEvent(cc);
	this->PutEvent(nnce);
}

void Game::Life()
{
	GetGraphics()->ShowLoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f);
	GetGraphics()->CreateSkyBox("Media/StarMap.dds"); 
	GetGraphics()->GetCamera()->SetPosition(Vector3(25, 25, 20));
	GetGraphics()->GetCamera()->LookAt(Vector3(0, 0, 0));

	//iLight* li = GetGraphics()->CreateLight(GetGraphics()->GetCamera()->GetPosition());
	//li->SetIntensity(0.001f);
	GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1), Vector3(1, 1, 1), 1.5f);
	GetGraphics()->SetSceneAmbientLight(Vector3(0.4f, 0.4f, 0.4f));
	iMesh* model = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(15, 20, 20));
	model->Scale(1.0f * 0.05f);

	GetGraphics()->LoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f, 1.0f, 1.0f);

	bool go = true;
	GetGraphics()->Update();
	while(GetGraphics()->IsRunning() && go)
	{
		// Updates GFX
		float diff = GetGraphics()->Update();	


		// Take care of packets / events
		if(MaloW::ProcessEvent* ev = this->PeekEvent())
		{
			// Network packet containing data
			if(MaloW::NetworkPacket* np = dynamic_cast<MaloW::NetworkPacket*>(ev))
			{
				string msg = np->getMessage();
				cc->sendData(msg);
				//if(msg == "PING")
				//	cc->sendData("PING");
			}

			// New client connected
			if(NewNetworkClientEvent* nnce = dynamic_cast<NewNetworkClientEvent*>(ev))
			{
				if(this->cc)
				{
					MaloW::Debug("Dropping current connection due to a new one incomming.");
					delete this->cc;
					this->cc = NULL;
				}
				this->cc = nnce->GetCC();
			}

			delete ev;
		}


		// 3D / game related.
		// Handle Keyboard inputs.
		if(GetGraphics()->GetKeyListener()->IsPressed('W'))
			GetGraphics()->GetCamera()->MoveForward(diff * 10.0f);
		if(GetGraphics()->GetKeyListener()->IsPressed('A'))
			GetGraphics()->GetCamera()->MoveLeft(diff * 10.0f);
		if(GetGraphics()->GetKeyListener()->IsPressed('S'))	
			GetGraphics()->GetCamera()->MoveBackward(diff * 10.0f);
		if(GetGraphics()->GetKeyListener()->IsPressed('D'))	
			GetGraphics()->GetCamera()->MoveRight(diff * 10.0f);

		if(GetGraphics()->GetKeyListener()->IsPressed(VK_ADD))
			GetGraphics()->GetCamera()->SetSpeed(GetGraphics()->GetCamera()->GetSpeed() * (1.0f + diff * 0.01f));
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_SUBTRACT))
			GetGraphics()->GetCamera()->SetSpeed(GetGraphics()->GetCamera()->GetSpeed() * (1.0f - diff * 0.01f));

		if(GetGraphics()->GetKeyListener()->IsPressed(VK_SPACE))
			GetGraphics()->GetCamera()->MoveUp(diff * 10.0f);
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_CONTROL))
			GetGraphics()->GetCamera()->MoveDown(diff * 10.0f);

		if(GetGraphics()->GetKeyListener()->IsPressed(VK_ESCAPE))
			go = false;
	}
}
