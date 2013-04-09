#pragma once

#include "MaloWLib/NetworkServer.h"
#include "Game.h"

class ConnectionListener : public MaloW::NetworkServer
{
private:
	Game* game;

public:
	ConnectionListener(int port, Game* game) : MaloW::NetworkServer(port)
	{
		this->game = game;
	}

	virtual ~ConnectionListener()
	{

	}

	virtual void ClientConnected(MaloW::ClientChannel* cc)
	{
		this->game->NewNetworkClient(cc);
	}
};