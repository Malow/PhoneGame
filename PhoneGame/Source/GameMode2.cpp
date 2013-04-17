#include "Game.h"
#include "Map.h"
#include "PowerBall.h"

#define PI 3.1415

void Game::PlayGameMode2()
{
	GraphicsEngine* mGe = GetGraphics();
	
	GetGraphics()->ShowLoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f);
	GetGraphics()->CreateSkyBox("Media/StarMap.dds"); 

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

	bool go = true;
	const Vector3 DefaultDir = Vector3(0.0f, 0.0f, 1.0f);
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


		if(this->networkController)
		{
			//Vector3 phoneDirr = Vector3(this->networkController->direction.y,-this->networkController->direction.z,this->networkController->direction.x);
			Vector3 phoneDirr = this->networkController->direction;
			phoneDirr.Normalize();
			float dot = phoneDirr.GetDotProduct(DefaultDir);
			acos(dot);
			Vector3 vecX = Vector3(phoneDirr.x, phoneDirr.y, phoneDirr.z);
			mPlatform->ResetXZAngles();

			if(phoneDirr.x > 0.2 || phoneDirr.x < -0.2 )
				mPlatform->RotateZ(phoneDirr.x * 400);
			if(phoneDirr.z > 0.2 || phoneDirr.z < -0.2 )
				mPlatform->RotateX(phoneDirr.z * 400);
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
	for(int i = 0; i < 5; i++)
		mGe->DeleteLight(mLights[i]);

	delete mPlatform;

	mGe->DeleteText(scoreTxt);
	mGe->DeleteText(timeTxt);
}