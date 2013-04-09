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



class Game : public MaloW::Process
{
private:
	MaloW::ClientChannel* cc;

public:
	Game();
	~Game();

	void NewNetworkClient(MaloW::ClientChannel* cc);
	void Life();
};

