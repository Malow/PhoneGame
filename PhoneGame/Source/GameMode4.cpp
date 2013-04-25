#include "Game.h"

#define NR_OF_HUMANS 10
static const Vector3 humanPos[NR_OF_HUMANS] = 
{
	Vector3(-110, 0.0f, 60),
	Vector3(-120, 40, -95),
	Vector3(-85, 40, -63),
	Vector3(-81, 24, 50),
	Vector3(-138, 0.0f, -77),
	Vector3(-160, 0.0f, -85),
	Vector3(-203, 0.0f, -35),
	Vector3(-236, 0.0f, 16),
	Vector3(-165, 0.0f, 75),
	Vector3(-170, 0.0f, -15)
};

static const Vector3 humanWayPoint[NR_OF_HUMANS] = 
{
	Vector3(-114, 0.0f, 0),
	Vector3(-120, 40, -95),
	Vector3(-122, 40, -63),
	Vector3(-81, 24, 50),
	Vector3(-145, 0.0f, 14),
	Vector3(-160, 0.0f, -50),
	Vector3(-203, 0.0f, -35),
	Vector3(-210, 0.0f, -30),
	Vector3(-165, 0.0f, 75),
	Vector3(-170, 0.0f, -15)
};

#define NR_OF_TREES 6
static const Vector3 treePos[NR_OF_TREES] = 
{
	Vector3(-107, 0.0f, -32),
	Vector3(-106, 0.0f, 31),
	Vector3(-225, 0.0f, 15),
	Vector3(-220, 0.0f, 1),
	Vector3(-212, 0.0f, 15),
	Vector3(-166, 0.0f, 122)
};

#define NR_OF_PREVPHONEDIR 20
static Vector3 prevPhoneDir[NR_OF_PREVPHONEDIR];


void Game::PlayGameMode4()
{
	GetGraphics()->ShowLoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f);

	float prevMouseSens = GetGraphics()->GetEngineParameters().MouseSensativity;
	float prevFOV = GetGraphics()->GetEngineParameters().FOV;
	int prevShadowQual = GetGraphics()->GetEngineParameters().ShadowMapSettings;

	// Score / results:
	float time = 0.0f;
	int score = 0;
	bool started = false;

	bool isScopedIn = false;
	bool scopeIn = false;
	bool shoot = false;
	bool moveLeft = false;
	bool moveRight = false;
	float shootTimer = 0.0f;
	float yRecoil = 0.0f;
	float recoilTimer = 0.0f;
	float mouseX = 0.0f;
	float mouseY = 0.0f;
	bool humanAlive[NR_OF_HUMANS];
	float humanRespawnTimer[NR_OF_HUMANS];
	bool humanWayPointForward[NR_OF_HUMANS];
	for(int i = 0; i < NR_OF_HUMANS; i++)
	{
		humanAlive[i] = true;
		humanRespawnTimer[i] = 5.0f;
		humanWayPointForward[i] = true;
	}


	bool firstPhoneDir = true;
	int phoneDirPlace = 0;
	float phoneDirTimer = 0.5f;
	float phoneScopeTimer = 0.5f;

	Vector3 phoneDir = Vector3(0, 1, 0);	
	Vector2 phoneAim = Vector2(0, 0);
	
	GetGraphics()->ChangeSkyBox("Media/skymap.dds");
	GetGraphics()->ChangeCamera(FPS);
	GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1), Vector3(1, 1, 1), 1.5f);
	GetGraphics()->SetSceneAmbientLight(Vector3(0.4f, 0.4f, 0.4f));
	GetGraphics()->ChangeShadowQuality(4);
	GetGraphics()->UseShadow(true);

	iMesh* model = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(15, 20, 20));
	model->Scale(1.0f * 0.05f);


	/// Trees and buildings
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
		i++;	// Every other
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



		//////////////////////////////////////////////////////////////////////////
		// Cheat controls
		/*
		if(GetGraphics()->GetKeyListener()->IsPressed('W'))
			GetGraphics()->GetCamera()->MoveForward(diff * 10.0f);
		if(GetGraphics()->GetKeyListener()->IsPressed('S'))	
			GetGraphics()->GetCamera()->MoveBackward(diff * 10.0f);
			*/
		//////////////////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////////////////
		// Keyboard / mouse inputs
		// Scope in
		static bool mouse2b = true;
		if(GetGraphics()->GetKeyListener()->IsClicked(2))
		{
			if(mouse2b)
			{
				scopeIn = true;
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
				shoot = true;
				mouse1b = false;
			}			
		}
		else
			mouse1b = true;

		// A and D movement
		if(GetGraphics()->GetKeyListener()->IsPressed('A'))
			moveLeft = true;
		if(GetGraphics()->GetKeyListener()->IsPressed('D'))	
			moveRight = true;
		//////////////////////////////////////////////////////////////////////////


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

			// Store and access previous phone dirs
			if(firstPhoneDir && phoneDir != Vector3(0, 0, 0) && phoneDir != Vector3(0, 1, 0))
			{
				for(int i = 0; i < NR_OF_PREVPHONEDIR; i++)
					prevPhoneDir[i] = phoneDir;
				firstPhoneDir = false;
			}

			if(!firstPhoneDir)
				phoneDirTimer -= diff * 0.001f;

			if(phoneDirTimer < 0.0f)
			{
				prevPhoneDir[phoneDirPlace] = phoneDir;
				phoneDirPlace++;
				if(phoneDirPlace == NR_OF_PREVPHONEDIR)
					phoneDirPlace = 0;
				phoneDirTimer = 0.5f;
			}

			Vector3 avgPhoneDir;
			for(int i = 0; i < NR_OF_PREVPHONEDIR; i++)
				avgPhoneDir += prevPhoneDir[i];
			avgPhoneDir /= NR_OF_PREVPHONEDIR;



			phoneAim = this->networkController->aim;
			float phoneAimLength = phoneAim.GetLength();
			if(phoneAimLength > 0.001f)	// Otherwise phoneAim is 0, 0
			{							
				// in -150 - 150, semi-normalized vector (meaning the highest both can be at the same time is around 105, 105)
				float x = phoneAim.x;
				float y = phoneAim.y;

				// -1 - 1 now, still circular like above.
				x /= 150.0f;
				y /= 150.0f;


				// Clamp to 0 if the value is too small, to create a deadzone. Value now is between -0.9 - 0.9
				static const float deadZone = 0.1f;
				if(abs(x) < deadZone)
					x = 0.0f;
				else
				{
					if(x < 0.0f)
						x += deadZone;
					else
						x -= deadZone;
				}

				if(abs(y) < deadZone)
					y = 0.0f;
				else
				{
					if(y < 0.0f)
						y += deadZone;
					else
						y -= deadZone;
				}


				// Exponential increase
				x *= abs(x);
				y *= abs(y);

				
				x *= 10.0f;
				y *= 10.0f;


				Vector2 mousePos = Vector2(GetGraphics()->GetEngineParameters().WindowWidth / 2, GetGraphics()->GetEngineParameters().WindowHeight / 2);
				if(isScopedIn)
				{
					mouseX += x * diff * 0.5f;
					mouseY += y * diff * 0.5f;
				}
				else
				{
					mouseX += x * diff;
					mouseY += y * diff;
				}
				Vector2 offset = Vector2(0, 0);
				offset.x += (int)mouseX;
				offset.y += (int)mouseY;
				mouseY -= (int)mouseY;
				mouseX -= (int)mouseX;
				
				GetGraphics()->GetKeyListener()->SetMousePosition(mousePos + offset);
			}

			
			// shoot
			if(this->networkController->shoot)
			{
				this->networkController->shoot = false;
				shoot = true;
			}

			
			// scopein
			phoneScopeTimer -= diff * 0.001f;
			if(phoneScopeTimer < 0.0f)
				phoneScopeTimer = 0.0f;
			Vector3 crossAvg = avgPhoneDir.GetCrossProduct(Vector3(0, 1, 0));
			float dotAvg = crossAvg.GetDotProduct(phoneDir);
			static bool phone1b = true;
			if(dotAvg > 0.55f && phoneScopeTimer == 0.0f)
			{
				if(phone1b)
				{
					scopeIn = true;
					phoneScopeTimer = 0.5f;
					phone1b = false;
				}			
			}
			else
				phone1b = true;
			

			// Move left and right
			if(this->networkController->direction.y < avgPhoneDir.y - 0.35f)
				moveLeft = true;
			if(this->networkController->direction.y > avgPhoneDir.y + 0.35f)
				moveRight = true;
		}

		//////////////////////////////////////////////////////////////////////////
		// GAME LOGIC
		for(int i = 0; i < NR_OF_HUMANS; i++)
		{
			// respawn dead humans
			if(!humanAlive[i])
			{
				humanRespawnTimer[i] -= diff * 0.001f;
				if(humanRespawnTimer[i] < 0.0f)
				{
					humanAlive[i] = true;
					humans[i]->DontRender(false);
					humanRespawnTimer[i] = 5.0f;
					humans[i]->SetPosition(humanPos[i]);
				}
			}
			// Move to and from waypoint
			else
			{
				Vector3 to;
				if(humanWayPointForward[i])
					to = humanWayPoint[i] - humanPos[i];
				else
					to = humanPos[i] - humanWayPoint[i];

				float toLength = to.GetLength();
				to.Normalize();

				humans[i]->MoveBy(to * diff * 0.01f);

				if((humans[i]->GetPosition() - humanWayPoint[i]).GetLength() > toLength)
					humanWayPointForward[i] = true;

				if((humans[i]->GetPosition() - humanPos[i]).GetLength() > toLength)
					humanWayPointForward[i] = false;
			}
		}


		shootTimer -= diff * 0.001f;
		if(shootTimer < 0.0f)
			shootTimer = 0.0f;

		recoilTimer -= diff * 0.001f;
		if(recoilTimer < 0.0f)
			recoilTimer = 0.0f;	

		// Move left and right
		if(moveLeft)
			GetGraphics()->GetCamera()->SetPosition(GetGraphics()->GetCamera()->GetPosition() + Vector3(0, 0, -diff) * 0.01f);
		if(moveRight)	
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
		moveLeft = false;
		moveRight = false;

		//Recoil up
		if(recoilTimer > 1.0f)
		{
			Vector2 mousePos = Vector2(GetGraphics()->GetEngineParameters().WindowWidth / 2, GetGraphics()->GetEngineParameters().WindowHeight / 2);
			GetGraphics()->GetKeyListener()->SetMousePosition(mousePos + Vector2(0, -diff * 10.0f));
		}
		// recoil down at normal speed
		else if(recoilTimer > 0.0f)
		{
			Vector2 mousePos = Vector2(GetGraphics()->GetEngineParameters().WindowWidth / 2, GetGraphics()->GetEngineParameters().WindowHeight / 2);
			GetGraphics()->GetKeyListener()->SetMousePosition(mousePos + Vector2(0, diff));
		}

		if(scopeIn)
		{
			if(!isScopedIn)
			{
				scope->SetOpacity(1.0f);
				sniper->SetOpacity(0.0f);
				GetGraphics()->GetEngineParameters().FOV = prevFOV * 0.33f;
				GetGraphics()->GetEngineParameters().MouseSensativity = prevMouseSens * 0.3f;
				isScopedIn = true;
			}
			else
			{
				scope->SetOpacity(0.0f);
				sniper->SetOpacity(1.0f);
				GetGraphics()->GetEngineParameters().FOV = prevFOV;
				GetGraphics()->GetEngineParameters().MouseSensativity = prevMouseSens;
				isScopedIn = false;
			}

			scopeIn = false;
		}


		if(shoot)
		{
			if(score == 0)
				started = true;

			if(shootTimer == 0.0f)
			{
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
				shootTimer = 1.0f;
				recoilTimer = 1.1f;
			}

			shoot = false;
		}

		if(started)
			time += diff * 0.001f;

		starTimer -= diff * 0.001f;
		if(starTimer < 0.0f)
			starTimer = 0.0f;
		guiStar->SetOpacity(starTimer);
		

		scoreTxt->SetText(string("SCORE: " + MaloW::convertNrToString(score)).c_str());
		timeTxt->SetText(string("TIME: " + MaloW::convertNrToString((int)time)).c_str());

		// End game after 2 mins
		if(time > 120.0f)
		{
			this->FinishScreen(score, "3, Sniper", time);
			go = false;
		}
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
	
	GetGraphics()->GetEngineParameters().MouseSensativity = prevMouseSens;
	GetGraphics()->GetEngineParameters().FOV = prevFOV;
	GetGraphics()->ChangeShadowQuality(prevShadowQual);
}

// TODO:
// Add ammo / reload??

// Look into more precise mouse stuff? Like reading the DPI of the mouse.

// Make all game modes stop after 120 sec and record score.