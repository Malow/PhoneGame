#include "Game.h"
#include "Map.h"
#include "PowerBall.h"
#include "GameObject.h"

#define START_POS Vector3(-13.0f,27,-13)

static const Vector3 scorePos[15] = 
{
	Vector3(-13.0f,27,-13), // Start pos
	Vector3(-8.33696, 25, -9.35953),
	Vector3(-4.30064, 25, -10.8502),
	Vector3(5.39558, 25, -12.7168),
	Vector3(13.7062, 25, -6.46825),
	Vector3(4.55834, 25, -2.98388),
	Vector3(-10.6329, 25, -3.1082),
	Vector3(2.04798, 25, 2.20836),
	Vector3(13.95, 25, 3.94194),
	Vector3(4.91268, 25, 6.56377),
	Vector3(-4.83424, 25, 6.15924),
	Vector3(-10.51, 25, 12.7038),
	Vector3(-1.22922, 25, 13.9699),
	Vector3(4.91268, 25, 13.9699),
	Vector3(13.95, 25, 13.9699)
};


#define PI 3.1415
#define NROFPREV 1
float CalcAngle(Vector3 &phoneDirr, Vector3 DefaultDir, Vector3 prevVectors[], int current)
{
	float totAngles = 0;
	float returnAngle;
	int i = current + 1;
	if(i == NROFPREV)
		i = 0;
	while(i != current)
	{
		phoneDirr += prevVectors[i];
		float dot = prevVectors[i].GetDotProduct(DefaultDir);
		float angles = acos(dot);
		totAngles += angles;

		i++;
		if(i == NROFPREV)
			i = 0;
	}
	phoneDirr /= NROFPREV;
	phoneDirr.Normalize();
	phoneDirr = (phoneDirr + prevVectors[current]) / 2;
	phoneDirr.Normalize();

	returnAngle = totAngles / NROFPREV;
	returnAngle = (returnAngle + acos(prevVectors[current].GetDotProduct(DefaultDir))) / 2;
	return returnAngle;
}

void Game::PlayGameMode2()
{
	GraphicsEngine* mGe = GetGraphics();
	
	GetGraphics()->ShowLoadingScreen("Media/LoadingScreen/LoadingScreenBG2.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f);
	GetGraphics()->ChangeSkyBox("Media/StarMap.dds"); 

	GetGraphics()->GetCamera()->SetUpVector(Vector3(0, 1, 0));
	GetGraphics()->GetCamera()->SetForward(Vector3(1, 0, 0));
	GetGraphics()->GetCamera()->SetPosition(Vector3(-17.0f, 56.0f, 0));
	GetGraphics()->GetCamera()->LookAt(GetGraphics()->GetCamera()->GetPosition() - Vector3(1, 0.3f, 0));

	iLight* mLights[5];
	mLights[0] = mGe->CreateLight(Vector3(0, 50, 0));
	mLights[1] = mGe->CreateLight(Vector3(0, 50, -20)); 
	mLights[2] = mGe->CreateLight(Vector3(0, 50, 20));
	mLights[3] = mGe->CreateLight(Vector3(10, 50, 0));
	mLights[4] = mGe->CreateLight(Vector3(-10, 50, 0));
	for(int i = 0; i < 5; i++)
		mLights[i]->SetIntensity(30.0f);

	GetGraphics()->SetSunLightDisabled();
	GetGraphics()->SetSceneAmbientLight(Vector3(0.4f, 0.4f, 0.4f));

	Vector3 centerPlatform = Vector3(0,20,0);
	Map* mPlatform = new Map("Media/MazeMapFixed.obj", centerPlatform);
	Map* mBox = new Map("Media/MazeMapFrame.obj", centerPlatform + Vector3(0,1,0) );
	mPlatform->SetShrinkValue(0.0f);

	/* set so we cant tilt it more than these angles. */
	mPlatform->SetMaxAngleX(10.0f*(PI/180.0f));
	mPlatform->SetMaxAngleZ(10.0f*(PI/180.0f));
	mPlatform->SetRotate(false);
	mPlatform->SetTargetAngleX(0.5f);
	mPlatform->SetTargetAngleZ(-0.5f);

	PowerBall* mBalls = new PowerBall("Media/Ball.obj", START_POS);
	mBalls->SetForwardVector(Vector3(0,0,1));
	mBalls->SetKnockoutMode();
	mBalls->SetAcceleration(mBalls->GetAcceleration()*30.0f);


	iText* timeTxt = GetGraphics()->CreateText("", Vector2(50, 60), 1.0f, "Media/fonts/new");
	iText* scoreTxt = GetGraphics()->CreateText("", Vector2(50, 95), 1.0f, "Media/fonts/new");

	iImage* guiStar = GetGraphics()->CreateImage(Vector2(200, 90), Vector2(75, 75), "Media/star.png");
	guiStar->SetOpacity(0.0f);
	float starTimer = 0.0f;


	GetGraphics()->LoadingScreen("Media/LoadingScreen/LoadingScreenBG2.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f, 1.0f, 1.0f);

	mGe->GetCamera()->SetPosition(centerPlatform + Vector3(0.0f, 30.0f, 20.0f));
	mGe->GetCamera()->LookAt(centerPlatform);

	// Score / results:
	float time = 0.0f;
	bool started = false;
	int score = 0;

	float delayTimer = 1000.0f;



	go = true;
	const Vector3 DefaultDir = Vector3(0.0f, 1.0f, 0.0f);
	int currentPrev = 0;
	Vector3 prevVectors[NROFPREV];
	for(int i = 0; i < NROFPREV; i++)
	{
		prevVectors[i] = DefaultDir;
	}

	while(delayTimer > 0)
	{
		float diff = GetGraphics()->Update();
		delayTimer -= diff;
	}

	while(GetGraphics()->IsRunning() && go)
	{
		if(mGe->GetKeyListener()->IsPressed('1'))
		{
			mGe->GetCamera()->SetPosition(centerPlatform+ Vector3(20.0f, 30.0f, 20.0f));
			mGe->GetCamera()->LookAt(centerPlatform );
		}
		if(mGe->GetKeyListener()->IsPressed('2'))
		{
			mGe->GetCamera()->SetPosition(centerPlatform+ Vector3(20.0f, 30.0f, -20.0f));
			mGe->GetCamera()->LookAt(centerPlatform );
		}
		if(mGe->GetKeyListener()->IsPressed('3'))
		{
			mGe->GetCamera()->SetPosition(centerPlatform+ Vector3(-20.0f, 30.0f, 20.0f));
			mGe->GetCamera()->LookAt(centerPlatform );
		}
		if(mGe->GetKeyListener()->IsPressed('4'))
		{
			mGe->GetCamera()->SetPosition(centerPlatform+ Vector3(-20.0f, 30.0f, -20.0f));
			mGe->GetCamera()->LookAt(centerPlatform );
		}

		// Updates GFX
		float diff = GetGraphics()->Update();

		// Handle events such as network packets and client connections
		this->HandleEvent(diff);
	
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_ESCAPE))
			go = false;
		if(GetGraphics()->GetKeyListener()->IsPressed('R') || (this->networkController != NULL && this->networkController->needRestart == true))
		{
			if(this->networkController != NULL)
			{
				this->networkController->needRestart = false;
			}
			delete mBalls;

			// Spawn at last score - 1;
			if(score == 0)
				mBalls = new PowerBall("Media/Ball.obj", scorePos[0]);
			else
				mBalls = new PowerBall("Media/Ball.obj", scorePos[score - 1]);

			mBalls->SetForwardVector(Vector3(0,0,1));
			mBalls->SetKnockoutMode();
			mBalls->SetAcceleration(mBalls->GetAcceleration()*15.0f);

			mPlatform->ResetXZAngles();
			mPlatform->RotateX(0);
		}
		iPhysicsEngine* pe = GetGraphics()->GetPhysicsEngine();
		mBalls->UpdateBallParentMode(mPlatform);
		mBalls->Update(diff);
		Vector3 normalPlane;
		if(pe->DoSpecialPhoneCollisionGame(mBalls, mPlatform, normalPlane, diff))
			mBalls->collisionPlatformResponse(mPlatform, normalPlane, diff);
		mBalls->UpdatePost();
		mPlatform->Update(diff);

		if(this->networkController)
		{
			mPlatform->SetRotate(true);
			Vector3 phoneDirr = Vector3(this->networkController->direction.y, this->networkController->direction.z, -this->networkController->direction.x);
			//Vector3 phoneDirr = this->networkController->direction;
			phoneDirr.Normalize();
			prevVectors[currentPrev] = phoneDirr;
			float angle = CalcAngle(phoneDirr, DefaultDir, prevVectors, currentPrev);

			float angleX = acos(DefaultDir.GetDotProduct(Vector3(0, phoneDirr.y , phoneDirr.z).Normalize()));
			float angleZ = acos(DefaultDir.GetDotProduct(Vector3(phoneDirr.x, phoneDirr.y, 0).Normalize()));
			if(phoneDirr.z > 0)
			{
				angleX *= -1;
			}
			if(phoneDirr.x < 0)
			{
				angleZ *= -1;
			}
			mPlatform->SetTargetAngleX(angleX/4);
			mPlatform->SetTargetAngleZ(angleZ/4);

			angle /= 4;
			currentPrev++;
			if(currentPrev >= NROFPREV)
				currentPrev = 0;
		}
		if(mGe->GetKeyListener()->IsPressed('W'))
		{
			mPlatform->RotateX(-diff);

			Vector3 tempPos = mBalls->GetPosition() - mPlatform->GetMesh()->GetPosition();
			tempPos.RotateAroundAxis(Vector3(1,0,0), (PI/8.0f)*-diff*0.001f);
			mBalls->SetPosition(mPlatform->GetMesh()->GetPosition() + tempPos);
		}
		if(mGe->GetKeyListener()->IsPressed('S'))
		{
			mPlatform->RotateX(diff);

			Vector3 tempPos = mBalls->GetPosition() - mPlatform->GetMesh()->GetPosition();
			tempPos.RotateAroundAxis(Vector3(1,0,0), (PI/8.0f)*diff*0.001f);
			mBalls->SetPosition(mPlatform->GetMesh()->GetPosition() + tempPos);
		}
		if(mGe->GetKeyListener()->IsPressed('A'))
		{
			mPlatform->RotateZ(-diff);

			Vector3 tempPos = mBalls->GetPosition() - mPlatform->GetMesh()->GetPosition();
			tempPos.RotateAroundAxis(Vector3(0,0,1), (PI/8.0f)*-diff*0.001f);
			mBalls->SetPosition(mPlatform->GetMesh()->GetPosition() + tempPos);
		}
		if(mGe->GetKeyListener()->IsPressed('D'))
		{
			mPlatform->RotateZ(diff);

			Vector3 tempPos = mBalls->GetPosition() - mPlatform->GetMesh()->GetPosition();
			tempPos.RotateAroundAxis(Vector3(0,0,1), (PI/8.0f)*diff*0.001f);
			mBalls->SetPosition(mPlatform->GetMesh()->GetPosition() + tempPos);
		}
		
		//////////////////////////////////////////////////////////////////////////
		static bool posb = true;
		if(GetGraphics()->GetKeyListener()->IsPressed('Q'))
		{
			if(posb)
			{
				MaloW::Debug(mBalls->GetPosition());
				posb = false;
			}			
		}
		else
			posb = true;
		//////////////////////////////////////////////////////////////////////////

		if(started)
		{
			for(int i = score + 1; i < 15; i++)
			{
				Vector3 toScore = scorePos[i] - mBalls->GetPosition();
				toScore.y = 0.0f;
				float distanceToScore = toScore.GetLength();
				if(distanceToScore < 2.0f)
				{
					score = i;
					starTimer = 2.0f;
				}
			}

			time += diff * 0.001f;
		}
		else
		{
			if((mBalls->GetPosition() - Vector3(-13.0f,25,-13)).GetLength() > 3)
			{
				started = true;
			}
		}

		starTimer -= diff * 0.001f;
		if(starTimer < 0.0f)
			starTimer = 0.0f;
		guiStar->SetOpacity(starTimer);

		// print score and time text
		scoreTxt->SetText(string("SCORE: " + MaloW::convertNrToString(score)).c_str());
		timeTxt->SetText(string("TIME: " + MaloW::convertNrToString(time)).c_str());


		// End game after 2 mins
		if(time > 120.0f || score > 13)
		{
			this->FinishScreen(score, "2, Labyrinth", time);
			go = false;
		}
	}

	if(this->networkController)
	{
		this->networkController->cc->sendData("QUITTING");
	}

	for(int i = 0; i < 5; i++)
		mGe->DeleteLight(mLights[i]);

	delete mPlatform;
	delete mBalls;
	delete mBox;

	mGe->DeleteText(timeTxt);
	mGe->DeleteText(scoreTxt);
	GetGraphics()->DeleteImage(guiStar);
}

// TODO