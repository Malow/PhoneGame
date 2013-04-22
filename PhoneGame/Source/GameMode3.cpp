#include "Game.h"

#include "GameMode3 Files\PhysMesh.h"
#include "GameMode3 Files\Helicopter.h"

#define HUMAN_POS_COUNT 10
#define YOFFSET 0.0f
static const Vector3 humanPos[HUMAN_POS_COUNT] = 
{
	Vector3(-114, YOFFSET, -112),
	Vector3(17, YOFFSET, 161),
	Vector3(-248, YOFFSET, -167),
	Vector3(-406, YOFFSET, -274),
	Vector3(-12, YOFFSET, -401),
	Vector3(-139, YOFFSET, 202),
	Vector3(-439, YOFFSET, 420),
	Vector3(-97, YOFFSET, -85),
	Vector3(-27, YOFFSET, 456),
	Vector3(-226, YOFFSET, 17)
};

void Game::PlayGameMode3()
{
	GetGraphics()->ShowLoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f);

	// Score / results:
	float time = 0.0f;
	int score = 0;
	bool started = false;


	GetGraphics()->ChangeSkyBox("Media/skymap.dds");
	GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1), Vector3(1, 1, 1), 1.5f);
	GetGraphics()->SetSceneAmbientLight(Vector3(0.4f, 0.4f, 0.4f));
	GetGraphics()->ChangeShadowQuality(4);
	GetGraphics()->UseShadow(true);

	iMesh* model = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(15, 20, 20));
	model->Scale(1.0f * 0.05f);

	GetGraphics()->GetCamera()->SetUpVector(Vector3(0, 1, 0));
	GetGraphics()->GetCamera()->SetForward(Vector3(1, 0, 0));
	GetGraphics()->GetCamera()->SetPosition(Vector3(100, 0, 100) + Vector3(0, 20, 0) - Vector3(1, 0, 0) * 40);
	GetGraphics()->GetCamera()->LookAt(Vector3(100, 0, 100) + Vector3(1, 0, 0) * 30 + Vector3(0, 10, 0));

	iTerrain* terrain = GetGraphics()->CreateTerrain(Vector3(0, 0, 0), Vector3(1500, 1, 1500), 256);
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
	terrain->SetTextureScale(15.0f);

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


	iText* timeTxt = GetGraphics()->CreateText("", Vector2(50, 65), 1.0f, "Media/fonts/new");
	iText* scoreTxt = GetGraphics()->CreateText("", Vector2(50, 95), 1.0f, "Media/fonts/new");


	iText* altitudeText = GetGraphics()->CreateText("", Vector2(50, 5), 1.0f, "Media/fonts/new");
	iText* speedText = GetGraphics()->CreateText("", Vector2(50, 30), 1.0f, "Media/fonts/new");

	iFBXMesh* human = GetGraphics()->CreateFBXMesh("Media/Token/token_anims.fbx", Vector3(100, 0, 110));
	human->SetScale(0.2f);
	human->SetPosition(humanPos[0]);

	iImage* guiStar = GetGraphics()->CreateImage(Vector2(200, 90), Vector2(75, 75), "Media/star.png");
	guiStar->SetOpacity(0.0f);
	float starTimer = 0.0f;

	iMesh* arrow = GetGraphics()->CreateMesh("Media/RedArrow.obj", Vector3(10, 2000, 15));

	Vector3 phoneDir = Vector3(0, 1, 0);
	GetGraphics()->LoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f, 1.0f, 1.0f);

	human->SetScale(0.2f);
	human->SetAnimation((unsigned int)0);
	human->HideModel("bow_v01", true);
	human->HideModel("machete_v3", true);
	human->HideModel("pocketknife_v01", true);
	human->HideModel("arrow_in_hand", true);
	human->HideModel("canteen", true);

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

		
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_UP))
			heli->camOffsetHeight += diff * 0.01f;
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_DOWN))
			heli->camOffsetHeight -= diff * 0.01f;

		if(GetGraphics()->GetKeyListener()->IsPressed(VK_ADD))
			heli->camOffsetDistance -= diff * 0.01f;
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_SUBTRACT))
			heli->camOffsetDistance += diff * 0.01f;


		if(this->networkController)
		{
			phoneDir = this->networkController->direction;
			phoneDir.Normalize();
			if(phoneDir == Vector3(0, 0, 0))
				phoneDir = Vector3(0, 1, 0);



			phoneDir.Normalize();
			Vector3 cross = heli->GetUpVector().GetCrossProduct(heli->GetForwardVector());
			cross.Normalize();
			float dotCrossPhone = cross.GetDotProduct(phoneDir);
			// Roll left
			if(dotCrossPhone < 0.0f)
			{
				heli->RollLeft(true);
				heli->RollRight(false);
			}
			// Roll right
			else if(dotCrossPhone > 0.0f)
			{
				heli->RollLeft(false);
				heli->RollRight(true);
			}

			float dotForPhone = heli->GetForwardVector().GetDotProduct(phoneDir);
			// Roll forward
			if(dotForPhone > 0.0f)
			{
				heli->RollForward(true);
				heli->RollBackward(false);
			}
			// Roll backward
			else if(dotForPhone < 0.0f)
			{
				heli->RollForward(false);
				heli->RollBackward(true);
			}

			
			float maxSpeed = heli->GetMaxRPM();
			float minSpeed = 0.0f;
			// min / max: 10 / 100: speed: 10 -> min(10) + input(10) * range(0.9) = 19
			float targetSpeed = minSpeed + this->networkController->speed * ((maxSpeed - minSpeed) * 0.01f);

			if(targetSpeed < minSpeed)
				targetSpeed = minSpeed;
			if(targetSpeed > maxSpeed)
				targetSpeed = maxSpeed;

			if(targetSpeed > heli->GetCurrentRPM())
			{
				heli->Accelerate(true);
				heli->Decelerate(false);
			}
			else
			{
				heli->Accelerate(false);
				heli->Decelerate(true);
			}
		}

		heli->Update(diff * 0.002f);	// double diff to get increased speed in gameplay


		altitudeText->SetText(string("ALTITUDE: " + MaloW::convertNrToString((int)heli->GetPos().y)).c_str());
		Vector3 dir = heli->GetDircetion();
		dir.y = 0.0f;
		float xzspeed = dir.GetLength();
		speedText->SetText(string("SPEED: " + MaloW::convertNrToString((int)xzspeed)).c_str());

		//////////////////////////////////////////////////////////////////////////
		// GAME LOGIC
		if(started)
			time += diff * 0.001f;

		float distance = (heli->GetPos() - human->GetPosition()).GetLength();
		if(distance < 15.0f && heli->GetPos().y < 1.0f && xzspeed < 1.0f)
		{
			if(score == 0)
				started = true;
			score++;
			human->SetPosition(humanPos[score % HUMAN_POS_COUNT]);

			starTimer = 2.0f;
		}
		starTimer -= diff * 0.001f;
		if(starTimer < 0.0f)
			starTimer = 0.0f;
		guiStar->SetOpacity(starTimer);
		

		scoreTxt->SetText(string("SCORE: " + MaloW::convertNrToString(score)).c_str());
		timeTxt->SetText(string("TIME: " + MaloW::convertNrToString((int)time)).c_str());


		// Update arrow pointing towards next game objective
		Vector3 VecToObjective =  human->GetPosition() - heli->GetPos();
		GetGraphics()->GetCamera()->SetMesh(arrow, Vector3(GetGraphics()->GetCamera()->GetForward() * -4 + GetGraphics()->GetCamera()->GetUpVector() * -1));
		arrow->ResetRotation();
		Vector3 vec = Vector3(0, -1, 0);
		Vector3 around = vec.GetCrossProduct(VecToObjective);
		float angle = -acos(vec.GetDotProduct(VecToObjective) / (vec.GetLength() * VecToObjective.GetLength()));
		arrow->RotateAxis(around, -angle);
		arrow->SetScale(log(1 + VecToObjective.GetLength()) * 0.03f);
		//////////////////////////////////////////////////////////////////////////
	}


	if(this->networkController)
	{
		this->networkController->cc->sendData("QUITTING");
	}

	GetGraphics()->GetCamera()->RemoveMesh();

	delete heli;
	GetGraphics()->DeleteMesh(model);
	GetGraphics()->DeleteMesh(chopper);
	GetGraphics()->DeleteMesh(rotor);
	GetGraphics()->DeleteMesh(secrotor);
	GetGraphics()->DeleteMesh(helipad);
	GetGraphics()->DeleteMesh(arrow);

	GetGraphics()->DeleteFBXMesh(human);

	GetGraphics()->DeleteTerrain(terrain);

	GetGraphics()->DeleteText(altitudeText);
	GetGraphics()->DeleteText(speedText);

	GetGraphics()->DeleteText(timeTxt);
	GetGraphics()->DeleteText(scoreTxt);

	GetGraphics()->DeleteImage(guiStar);
}

// TODO:
// Add a way to move the camera angle up and down.

// add texts for info
