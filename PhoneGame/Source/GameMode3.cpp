#include "Game.h"

#include "GameMode3 Files\PhysMesh.h"
#include "GameMode3 Files\Helicopter.h"

void Game::PlayGameMode3()
{
	GetGraphics()->ShowLoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f);
	GetGraphics()->ChangeSkyBox("Media/skymap.dds");
	GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1), Vector3(1, 1, 1), 1.5f);
	GetGraphics()->SetSceneAmbientLight(Vector3(0.4f, 0.4f, 0.4f));

	iMesh* model = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(15, 20, 20));
	model->Scale(1.0f * 0.05f);

	GetGraphics()->GetCamera()->SetUpVector(Vector3(0, 1, 0));
	GetGraphics()->GetCamera()->SetForward(Vector3(1, 0, 0));
	GetGraphics()->GetCamera()->SetPosition(Vector3(100, 0, 100) + Vector3(0, 20, 0) - Vector3(1, 0, 0) * 40);
	GetGraphics()->GetCamera()->LookAt(Vector3(100, 0, 100) + Vector3(1, 0, 0) * 30 + Vector3(0, 10, 0));

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

	iMesh* chopper = GetGraphics()->CreateMesh("Media/Apache_Maya_WO_SecRot.obj", Vector3(100, 0, 100));
	iMesh* rotor = GetGraphics()->CreateMesh("Media/Apache_Rotor.obj", Vector3(100, 0, 100));
	iMesh* secrotor = GetGraphics()->CreateMesh("Media/Apache_SecRotor.obj", Vector3(100, 0, 100));
	chopper->Scale(5.0f);
	rotor->Scale(5.0f);
	secrotor->Scale(4.0f);
	Helicopter* heli = NULL;
	heli = new Helicopter(chopper, rotor, secrotor, terrain);

	iMesh* helipad = GetGraphics()->CreateMesh("Media/Helipad.obj", Vector3(100, 0, 100));
	helipad->SetScale(3.0f);


	iText* altitudeText = GetGraphics()->CreateText("", Vector2(50, 5), 1.0f, "Media/fonts/new");
	iText* speedText = GetGraphics()->CreateText("", Vector2(50, 30), 1.0f, "Media/fonts/new");

	GetGraphics()->LoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f, 1.0f, 1.0f);

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


		heli->Update(diff * 0.002f);	// double diff to get increased speed in gameplay


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
		



		altitudeText->SetText(string("ALTITUDE: " + MaloW::convertNrToString((int)heli->GetPos().y)).c_str());
		Vector3 dir = heli->GetDircetion();
		dir.y = 0.0f;
		float speed = dir.GetLength();
		speedText->SetText(string("SPEED: " + MaloW::convertNrToString((int)speed)).c_str());
	}


	if(this->networkController)
	{
		this->networkController->cc->sendData("QUITTING");
	}


	delete heli;
	GetGraphics()->DeleteMesh(model);
	GetGraphics()->DeleteMesh(chopper);
	GetGraphics()->DeleteMesh(rotor);
	GetGraphics()->DeleteMesh(secrotor);
	GetGraphics()->DeleteMesh(helipad);

	GetGraphics()->DeleteTerrain(terrain);

	GetGraphics()->DeleteText(altitudeText);
	GetGraphics()->DeleteText(speedText);
}

// TODO:

// Come up with game-logic, what should the game be about?
//		Probably fly collect some stuff both in the air and on the ground?

// Add a way to move the camera angle up and down.

// Implement phone.

// add texts for info

// Make it so u cant roll on the ground and increase ground friction as well as ground distance friction