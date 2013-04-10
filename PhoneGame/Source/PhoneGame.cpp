#if defined(DEBUG) || defined(_DEBUG)
#include <vld.h>
#define INCLUDE_MODEL_VIEWER
#endif

#include "Graphics.h"
#include "..\Source\MaloWFileDebug.h"
#include "ConnectionListener.h"
#include "Game.h"

int __stdcall wWinMain( HINSTANCE hInstance, HINSTANCE, LPWSTR, int )
{
	MaloW::ClearDebug();
	if ( !GraphicsInit(hInstance) )
		MaloW::Debug("Failed Creating Graphics Engine!");
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	MaloW::Debug("(DEBUG): ModelViewer: Debug flag set to: _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF). ");
#ifdef INCLUDE_MODEL_VIEWER
	MaloW::Debug("(DEBUG): ModelViewer: vld.h included.");
#endif
#endif

	Game* game = new Game();
	ConnectionListener* cl = new ConnectionListener(10000, game);
	cl->Start();
	
	game->Play(1);

	game->Close(); 
	cl->Close();
	delete cl;
	delete game;

	FreeGraphics();
	return 0;
}