#pragma once

#include "Graphics.h"
#include "MaloWLib/Process.h"
#include "MaloWLib/ClientChannel.h"



class NewNetworkClientEvent : public MaloW::ProcessEvent
{
private:
	MaloW::ClientChannel* cc;
	bool selfDelete;

public:
	NewNetworkClientEvent(MaloW::ClientChannel* cc)
	{
		this->cc = cc;
		this->selfDelete = true;
	}

	virtual ~NewNetworkClientEvent()
	{
		if(this->selfDelete)
			delete this->cc;
	}

	MaloW::ClientChannel* GetCC() { this->selfDelete = false; return this->cc; }
};


class NetworkController
{
private:
	
public:
	MaloW::ClientChannel* cc;
	Vector3 direction;
	float speed;

	NetworkController()
	{
		this->cc = NULL;
		this->direction = Vector3(0, 0, 0);
		this->speed = 50.0f;
	}

	virtual ~NetworkController()
	{
		if(this->cc)
		{
			this->cc->Close();
			delete this->cc;
			this->cc = NULL;
		}
	}
};


class Game : public MaloW::Process
{
private:
	NetworkController* networkController;
	int gameMode;
	bool go;
	bool play;

	void HandleEvent(float diff);
	void PlayGameMode1();
	void PlayGameMode2();
	void PlayGameMode3();
	void PlayGameMode4();

public:
	Game();
	~Game();

	void NewNetworkClient(MaloW::ClientChannel* cc);
	void Life() { };
	void Play();
};

