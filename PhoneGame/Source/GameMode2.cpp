#include "Game.h"
#include "Map.h"
#include "PowerBall.h"

#define PI 3.1415
#define NROFPREV 100
float CalcAngle(Vector3 &phoneDirr, Vector3 DefaultDir, Vector3 prevVectors[])
{
	float totAngles = 0;
	for(int i = 0; i < NROFPREV; i++)
	{
		phoneDirr += prevVectors[i];
		float dot = prevVectors[i].GetDotProduct(DefaultDir);
		float angles = acos(dot);
		totAngles += angles;
	}
	phoneDirr /= NROFPREV;
	phoneDirr.Normalize();
	return totAngles / NROFPREV;
}

void Game::PlayGameMode2()
{
	GraphicsEngine* mGe = GetGraphics();
	
	GetGraphics()->ShowLoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f);
	GetGraphics()->ChangeSkyBox("Media/StarMap.dds"); 

	iLight* mLights[5];
	mLights[0] = mGe->CreateLight(Vector3(0, 50, 0));
	mLights[1] = mGe->CreateLight(Vector3(0, 50, -20)); 
	mLights[2] = mGe->CreateLight(Vector3(0, 50, 20));
	mLights[3] = mGe->CreateLight(Vector3(10, 50, 0));
	mLights[4] = mGe->CreateLight(Vector3(-10, 50, 0));
	for(int i = 0; i < 5; i++)
		mLights[i]->SetIntensity(30.0f);

	Vector3 centerPlatform = Vector3(0,20,0);
	Map* mPlatform = new Map("Media/MazeMap.obj", centerPlatform);
	Map* mBox = new Map("Media/MazeMapFrame.obj", centerPlatform + Vector3(0,1,0) );
	mPlatform->SetRotate(false);
	mPlatform->SetShrinkValue(0.0f);

	/* set so we cant tilt it more than these angles. */
	mPlatform->SetMaxAngleX(10.0f*(PI/180.0f));
	mPlatform->SetMaxAngleZ(10.0f*(PI/180.0f));

	PowerBall* mBalls = new PowerBall("Media/Ball.obj", Vector3(-13.0f,27,-13));
	mBalls->SetForwardVector(Vector3(0,0,1));
	mBalls->SetKnockoutMode();
	mBalls->SetAcceleration(mBalls->GetAcceleration()*3.0f);
	mBalls->SetForcePressed(mBalls->GetForcePressed()/15.0f);

	GetGraphics()->LoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f, 1.0f, 1.0f);

	mGe->GetCamera()->SetPosition(centerPlatform+ Vector3(0.0f, 30.0f, 20.0f));
	mGe->GetCamera()->LookAt(centerPlatform);

	// Score / results:
	float time = 0.0f;
	int score = 0;
	bool started = false;

	iText* timeTxt = GetGraphics()->CreateText("", Vector2(50, 60), 1.0f, "Media/fonts/new");
	iText* scoreTxt = GetGraphics()->CreateText("", Vector2(50, 90), 1.0f, "Media/fonts/new");

	go = true;
	const Vector3 DefaultDir = Vector3(0.0f, 1.0f, 0.0f);
	int currentPrev = 0;
	Vector3 prevVectors[NROFPREV];
	for(int i = 0; i < NROFPREV; i++)
	{
		prevVectors[i] = DefaultDir;
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
			Vector3 phoneDirr = Vector3(this->networkController->direction.y, this->networkController->direction.z, -this->networkController->direction.x);
			//Vector3 phoneDirr = this->networkController->direction;
			phoneDirr.Normalize();
			prevVectors[currentPrev] = phoneDirr;
			currentPrev++;
			if(currentPrev >= NROFPREV)
				currentPrev = 0;

			float angle = CalcAngle(phoneDirr, DefaultDir, prevVectors);
			
			if(angle > 0.5)
				angle = 0.5;
			if(angle < -0.5)
				angle = -0.5;
			mPlatform->ResetXZAngles();
			mPlatform->RotateAxis(phoneDirr.GetCrossProduct(DefaultDir), angle);
		}
		if(mGe->GetKeyListener()->IsPressed('W'))
			mPlatform->RotateX(-diff);
		if(mGe->GetKeyListener()->IsPressed('S'))
			mPlatform->RotateX(diff);
		if(mGe->GetKeyListener()->IsPressed('A'))
			mPlatform->RotateZ(-diff);
		if(mGe->GetKeyListener()->IsPressed('D'))
			mPlatform->RotateZ(diff);

		if(started)
			time += diff * 0.001f;

		// print score and time text.
		scoreTxt->SetText(string("SCORE: " + MaloW::convertNrToString(score)).c_str());
		timeTxt->SetText(string("TIME: " + MaloW::convertNrToString(time)).c_str());
	}

	if(this->networkController)
	{
		this->networkController->cc->sendData("QUITTING");
	}

	for(int i = 0; i < 5; i++)
		mGe->DeleteLight(mLights[i]);

	delete mPlatform;
	delete mBalls;

	mGe->DeleteText(scoreTxt);
	mGe->DeleteText(timeTxt);
}
