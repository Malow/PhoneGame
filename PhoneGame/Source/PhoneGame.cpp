#if defined(DEBUG) || defined(_DEBUG)
#include <vld.h>
#define INCLUDE_MODEL_VIEWER
#endif

#include "Graphics.h"
#include "..\Source\MaloWFileDebug.h"

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
		Sleep(10);

		// Updates camera etc, does NOT render the frame, another process is doing that, so diff should be very low.
		float diff = GetGraphics()->Update();	

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

	FreeGraphics();
	return 0;
}