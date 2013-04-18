#include "Game.h"

#include "GameMode3 Files\PhysMesh.h"
#include "GameMode3 Files\Helicopter.h"

void Game::PlayGameMode3()
{
	GetGraphics()->CreateSkyBox("Media/skymap.dds");

	iMesh* model = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(15, 20, 20));
	model->Scale(1.0f * 0.05f);


	GetGraphics()->GetCamera()->SetPosition(Vector3(100, 10, 100));

	iTerrain* terrain = GetGraphics()->CreateTerrain(Vector3(-100, -1, -100), Vector3(200, 1, 200), 256);


	iMesh* chopper = GetGraphics()->CreateMesh("Media/Apache_Maya_WO_SecRot.obj", Vector3(100, 50, 100));
	iMesh* rotor = GetGraphics()->CreateMesh("Media/Apache_Rotor.obj", Vector3(100, 50, 100));
	iMesh* secrotor = GetGraphics()->CreateMesh("Media/Apache_SecRotor.obj", Vector3(100, 50, 100));
	chopper->Scale(5.0f);
	rotor->Scale(5.0f);
	secrotor->Scale(4.0f);
	Helicopter* heli = NULL;
	heli = new Helicopter(chopper, rotor, secrotor, terrain);


	GetGraphics()->GetCamera()->SetMesh(chopper, Vector3(10, 10, 10));
	GetGraphics()->GetCamera()->LookAt(chopper->GetPosition());


	bool go = true;
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


		heli->Update(diff * 0.001f);


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