#include "Game.h"

#include "GameMode3 Files\PhysMesh.h"
#include "GameMode3 Files\Helicopter.h"

void Game::PlayGameMode3()
{
	GetGraphics()->CreateSkyBox("Media/skymap.dds");
	GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1), Vector3(1, 1, 1), 1.5f);
	GetGraphics()->SetSceneAmbientLight(Vector3(0.4f, 0.4f, 0.4f));

	iMesh* model = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(15, 20, 20));
	model->Scale(1.0f * 0.05f);


	GetGraphics()->GetCamera()->SetPosition(Vector3(100, 10, 100));

	iTerrain* terrain = GetGraphics()->CreateTerrain(Vector3(0, 0, 0), Vector3(1000, 1, 1000), 256);
	const char* fileNames[8];
	fileNames[0] = "Media/TerrainTexture.png";
	fileNames[1] = "Media/TerrainTexture.png";
	fileNames[2] = "Media/TerrainTexture.png";
	fileNames[3] = "Media/TerrainTexture.png";
	fileNames[4] = "Media/TerrainTexture.png";
	fileNames[5] = "Media/TerrainTexture.png";
	fileNames[6] = "Media/TerrainTexture.png";
	fileNames[7] = "Media/TerrainTexture.png";
	terrain->SetTextures(fileNames);
	terrain->SetTextureScale(10.0f);

	iMesh* chopper = GetGraphics()->CreateMesh("Media/Apache_Maya_WO_SecRot.obj", Vector3(100, 50, 100));
	iMesh* rotor = GetGraphics()->CreateMesh("Media/Apache_Rotor.obj", Vector3(100, 50, 100));
	iMesh* secrotor = GetGraphics()->CreateMesh("Media/Apache_SecRotor.obj", Vector3(100, 50, 100));
	chopper->Scale(5.0f);
	rotor->Scale(5.0f);
	secrotor->Scale(4.0f);
	Helicopter* heli = NULL;
	heli = new Helicopter(chopper, rotor, secrotor, terrain);

	go = true;
	GetGraphics()->Update();
	while(GetGraphics()->IsRunning() && go)
	{
		Sleep(1);
		// Updates GFX
		float diff = GetGraphics()->Update();	

		// Handle events such as network packets and client connections
		this->HandleEvent(diff);

		if(GetGraphics()->GetKeyListener()->IsPressed(VK_ESCAPE))
			go = false;


		heli->Update(diff * 0.002f);


		// Heli Controlls
		if(GetGraphics()->GetKeyListener()->IsPressed('Q'))
			heli->Accelerate(true);
		else
			heli->Accelerate(false);
		if(GetGraphics()->GetKeyListener()->IsPressed('E'))
			heli->Decelerate(true);
		else
			heli->Decelerate(false);

		if(GetGraphics()->GetKeyListener()->IsPressed('Z'))
			heli->YawLeft(true);
		else
			heli->YawLeft(false);
		if(GetGraphics()->GetKeyListener()->IsPressed('X'))
			heli->YawRight(true);
		else
			heli->YawRight(false);

		if(GetGraphics()->GetKeyListener()->IsPressed('W'))
			heli->RollForward(true);
		else
			heli->RollForward(false);
		if(GetGraphics()->GetKeyListener()->IsPressed('A'))
			heli->RollLeft(true);
		else
			heli->RollLeft(false);
		if(GetGraphics()->GetKeyListener()->IsPressed('S'))
			heli->RollBackward(true);
		else
			heli->RollBackward(false);
		if(GetGraphics()->GetKeyListener()->IsPressed('D'))
			heli->RollRight(true);
		else
			heli->RollRight(false);
		







	}

	delete heli;
	GetGraphics()->DeleteMesh(model);
	GetGraphics()->DeleteMesh(chopper);
	GetGraphics()->DeleteMesh(rotor);
	GetGraphics()->DeleteMesh(secrotor);
}

// TODO:

// change skybox crashes when one is allready created.
// Helicopter.cpp rad 109: GOGOGO!