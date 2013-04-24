#include "Game.h"

#define NR_OF_HUMANS 10
#define YOFFSET 0.0f
static const Vector3 humanPos[NR_OF_HUMANS] = 
{
	Vector3(-114, YOFFSET, 0),
	Vector3(-120, 40, -95),
	Vector3(-122, 40, -63),
	Vector3(-81, 24, 50),
	Vector3(-145, YOFFSET, 14),
	Vector3(-160, YOFFSET, -50),
	Vector3(-203, YOFFSET, -35),
	Vector3(-210, YOFFSET, -30),
	Vector3(-165, YOFFSET, 75),
	Vector3(-170, YOFFSET, -15)
};

#define NR_OF_TREES 6
static const Vector3 treePos[NR_OF_TREES] = 
{
	Vector3(-107, YOFFSET, -32),
	Vector3(-106, YOFFSET, 31),
	Vector3(-225, YOFFSET, 15),
	Vector3(-220, YOFFSET, 1),
	Vector3(-212, YOFFSET, 15),
	Vector3(-166, YOFFSET, 122)/*,
	Vector3(-439, YOFFSET, 420),
	Vector3(-97, YOFFSET, -85),
	Vector3(-27, YOFFSET, 456),
	Vector3(-226, YOFFSET, 17),
	Vector3(-114, YOFFSET, 0),
	Vector3(17, YOFFSET, 161),
	Vector3(-248, YOFFSET, -167),
	Vector3(-406, YOFFSET, -274),
	Vector3(-12, YOFFSET, -401),
	Vector3(-139, YOFFSET, 202),
	Vector3(-439, YOFFSET, 420),
	Vector3(-97, YOFFSET, -85),
	Vector3(-27, YOFFSET, 456),
	Vector3(-226, YOFFSET, 17)*/
};


void Game::PlayGameMode4()
{
	GetGraphics()->ShowLoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f);

	// Score / results:
	float time = 0.0f;
	int score = 0;
	bool started = false;

	bool scopedIn = false;
	bool humanAlive[NR_OF_HUMANS];
	for(int i = 0; i < NR_OF_HUMANS; i++)
		humanAlive[i] = true;

	Vector3 phoneDir = Vector3(0, 1, 0);	
	
	GetGraphics()->ChangeSkyBox("Media/skymap.dds");
	GetGraphics()->ChangeCamera(FPS);
	GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1), Vector3(1, 1, 1), 1.5f);
	GetGraphics()->SetSceneAmbientLight(Vector3(0.4f, 0.4f, 0.4f));
	GetGraphics()->ChangeShadowQuality(4);
	GetGraphics()->UseShadow(true);

	iMesh* model = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(15, 20, 20));
	model->Scale(1.0f * 0.05f);


	///
	iMesh* building1 = GetGraphics()->CreateMesh("Media/TallBuilding.obj", Vector3(0, -20.0f, 0));
	building1->Scale(Vector3(4, 4, 8));

	iMesh* building2 = GetGraphics()->CreateMesh("Media/TallBuilding.obj", Vector3(-75.0f, -30.0f, 50.0f));
	building2->Scale(3.0f);

	iMesh* building3 = GetGraphics()->CreateMesh("Media/TallBuilding.obj", Vector3(-105.0f, -50.0f, -80.0f));
	building3->Scale(5.0f);


	iMesh* trees[NR_OF_TREES];
	for(int i = 0; i < NR_OF_TREES; i++)
	{
		trees[i] = GetGraphics()->CreateMesh("Media/Tree_01_03.obj", treePos[i]);
		trees[i]->Scale(0.1f);
	}

	///


	GetGraphics()->GetCamera()->SetUpVector(Vector3(0, 1, 0));
	GetGraphics()->GetCamera()->SetForward(Vector3(1, 0, 0));
	GetGraphics()->GetCamera()->SetPosition(Vector3(-17.0f, 56.0f, 0));
	GetGraphics()->GetCamera()->LookAt(GetGraphics()->GetCamera()->GetPosition() - Vector3(1, 0.3f, 0));

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
	
	iText* timeTxt = GetGraphics()->CreateText("", Vector2(50, 65), 1.0f, "Media/fonts/newBorder");
	iText* scoreTxt = GetGraphics()->CreateText("", Vector2(50, 95), 1.0f, "Media/fonts/newBorder");

	iImage* guiStar = GetGraphics()->CreateImage(Vector2(200, 75), Vector2(75, 75), "Media/star.png");
	guiStar->SetStrata(400.0f);
	iImage* sniper = GetGraphics()->CreateImage(Vector2(0, 0), Vector2(GetGraphics()->GetEngineParameters().WindowWidth, GetGraphics()->GetEngineParameters().WindowHeight), "Media/AS50.png");
	iImage* scope = GetGraphics()->CreateImage(Vector2(0, 0), Vector2(GetGraphics()->GetEngineParameters().WindowWidth, GetGraphics()->GetEngineParameters().WindowHeight), "Media/scope.png");
	scope->SetOpacity(0.0f);
	guiStar->SetOpacity(0.0f);
	float starTimer = 0.0f;

#ifdef _DEBUG
	iText* phoneDirTxtX = GetGraphics()->CreateText("", Vector2(50, 155), 1.0f, "Media/fonts/newBorder");
	iText* phoneDirTxtY = GetGraphics()->CreateText("", Vector2(50, 185), 1.0f, "Media/fonts/newBorder");
	iText* phoneDirTxtZ = GetGraphics()->CreateText("", Vector2(50, 215), 1.0f, "Media/fonts/newBorder");
#endif

	iMesh* humans[NR_OF_HUMANS];
	for(int i = 0; i < NR_OF_HUMANS; i++)
	{
		humans[i] = GetGraphics()->CreateMesh("Media/temp_guy.obj", Vector3(0, 0, 0));
		humans[i]->SetScale(0.2f);
		humans[i]->SetPosition(humanPos[i]);
	}


	GetGraphics()->LoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f, 1.0f, 1.0f);

	
	for(int i = 0; i < NR_OF_HUMANS; i++)
	{
		humans[i]->RotateAxis(Vector3(0, 1, 0), -3.1415f * 0.5f);
		i++;
		/*
		humans[i]->SetScale(0.2f);
		humans[i]->SetAnimation((unsigned int)0);
		humans[i]->HideModel("bow_v01", true);
		humans[i]->HideModel("machete_v3", true);
		humans[i]->HideModel("pocketknife_v01", true);
		humans[i]->HideModel("arrow_in_hand", true);
		humans[i]->HideModel("canteen", true);
		*/
	}
	
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


#ifdef _DEBUG
		// Debug Controlls
		if(GetGraphics()->GetKeyListener()->IsPressed('W'))
			GetGraphics()->GetCamera()->MoveForward(diff * 10.0f);
		if(GetGraphics()->GetKeyListener()->IsPressed('S'))	
			GetGraphics()->GetCamera()->MoveBackward(diff * 10.0f);

		if(GetGraphics()->GetKeyListener()->IsPressed(VK_ADD))
			GetGraphics()->GetCamera()->SetSpeed(GetGraphics()->GetCamera()->GetSpeed() * (1.0f + diff * 0.01f));
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_SUBTRACT))
			GetGraphics()->GetCamera()->SetSpeed(GetGraphics()->GetCamera()->GetSpeed() * (1.0f - diff * 0.01f));

		if(GetGraphics()->GetKeyListener()->IsPressed(VK_SPACE))
			GetGraphics()->GetCamera()->MoveUp(diff * 10.0f);
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_CONTROL))
			GetGraphics()->GetCamera()->MoveDown(diff * 10.0f);
#endif


		if(this->networkController)
		{
			phoneDir = this->networkController->direction;
			phoneDir.Normalize();
			if(phoneDir == Vector3(0, 0, 0))
				phoneDir = Vector3(0, 1, 0);
			

#ifdef _DEBUG
			phoneDirTxtX->SetText(string("PHONEDIR: X: " + MaloW::convertNrToString(phoneDir.x)).c_str());
			phoneDirTxtY->SetText(string("PHONEDIR: Y: " + MaloW::convertNrToString(phoneDir.y)).c_str());
			phoneDirTxtZ->SetText(string("PHONEDIR: Z: " + MaloW::convertNrToString(phoneDir.z)).c_str());
#endif

		}

		//////////////////////////////////////////////////////////////////////////
		// GAME LOGIC

		// A and D movement
		if(GetGraphics()->GetKeyListener()->IsPressed('A'))
			GetGraphics()->GetCamera()->SetPosition(GetGraphics()->GetCamera()->GetPosition() + Vector3(0, 0, -diff) * 0.01f);
		if(GetGraphics()->GetKeyListener()->IsPressed('D'))	
			GetGraphics()->GetCamera()->SetPosition(GetGraphics()->GetCamera()->GetPosition() + Vector3(0, 0, diff) * 0.01f);
		if(GetGraphics()->GetCamera()->GetPosition().z > 34.0f)
		{
			Vector3 pos = GetGraphics()->GetCamera()->GetPosition();
			pos.z = 34.0f;
			GetGraphics()->GetCamera()->SetPosition(pos);
		}
		if(GetGraphics()->GetCamera()->GetPosition().z < -34.0f)
		{
			Vector3 pos = GetGraphics()->GetCamera()->GetPosition();
			pos.z = -34.0f;
			GetGraphics()->GetCamera()->SetPosition(pos);
		}


		// Zoom in
		static bool mouse2b = true;
		static int mouse2i = 0;
		if(GetGraphics()->GetKeyListener()->IsClicked(2))
		{
			if(mouse2b)
			{
				if(mouse2i % 2 == 0)
				{
					scope->SetOpacity(1.0f);
					sniper->SetOpacity(0.0f);
					GetGraphics()->GetEngineParameters().FOV = 25.0f;
					GetGraphics()->GetEngineParameters().MouseSensativity = 0.3f;
					scopedIn = true;
				}
				else
				{
					scope->SetOpacity(0.0f);
					sniper->SetOpacity(1.0f);
					GetGraphics()->GetEngineParameters().FOV = 75.0f;
					GetGraphics()->GetEngineParameters().MouseSensativity = 1.0f;
					scopedIn = false;
				}
				mouse2i++;
				mouse2b = false;
			}			
		}
		else
			mouse2b = true;


		// Shoot
		static bool mouse1b = true;
		if(GetGraphics()->GetKeyListener()->IsClicked(1))
		{
			if(mouse1b)
			{
				if(score == 0)
					started = true;

				// Do collision, if you hit something:
				Vector3 camFor = GetGraphics()->GetCamera()->GetForward();
				Vector3 camPos = GetGraphics()->GetCamera()->GetPosition();
				for(int i = 0; i < NR_OF_HUMANS; i++)
				{
					if(humanAlive[i])
					{
						CollisionData cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayMesh(camPos, camFor, humans[i]);
						if(cd.collision)
						{
							humanAlive[i] = false;
							humans[i]->DontRender(true);
							score++;
							starTimer = 2.0f;
							i = NR_OF_HUMANS;
						}
					}
				}

				mouse1b = false;
			}			
		}
		else
			mouse1b = true;



		if(started)
			time += diff * 0.001f;

		starTimer -= diff * 0.001f;
		if(starTimer < 0.0f)
			starTimer = 0.0f;
		guiStar->SetOpacity(starTimer);
		

		scoreTxt->SetText(string("SCORE: " + MaloW::convertNrToString(score)).c_str());
		timeTxt->SetText(string("TIME: " + MaloW::convertNrToString((int)time)).c_str());

		//////////////////////////////////////////////////////////////////////////
	}


	if(this->networkController)
	{
		this->networkController->cc->sendData("QUITTING");
	}

	GetGraphics()->GetCamera()->RemoveMesh();

	GetGraphics()->DeleteMesh(model);

	GetGraphics()->DeleteMesh(building1);
	GetGraphics()->DeleteMesh(building2);
	GetGraphics()->DeleteMesh(building3);

	for(int i = 0; i < NR_OF_TREES; i++)
	{
		GetGraphics()->DeleteMesh(trees[i]);
	}
	
	for(int i = 0; i < NR_OF_HUMANS; i++)
	{
		GetGraphics()->DeleteMesh(humans[i]);
	}

	GetGraphics()->DeleteTerrain(terrain);


	GetGraphics()->DeleteText(timeTxt);
	GetGraphics()->DeleteText(scoreTxt);

	GetGraphics()->DeleteImage(guiStar);
	GetGraphics()->DeleteImage(sniper);
	GetGraphics()->DeleteImage(scope);

#ifdef _DEBUG
	GetGraphics()->DeleteText(phoneDirTxtX);
	GetGraphics()->DeleteText(phoneDirTxtY);
	GetGraphics()->DeleteText(phoneDirTxtZ);
#endif
}

// TODO:

// elegant way of setting mousesens, FOV and shadow qual back to config file settings instead of hardcore, goes for heli game too.







