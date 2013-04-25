#include "Game.h"
#include "Map.h"
#include "PowerBall.h"
#include "GameObject.h"

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
	
	GetGraphics()->ShowLoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f);
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
	Map* mPlatform = new Map("Media/MazeMap.obj", centerPlatform);
	Map* mBox = new Map("Media/MazeMapFrame.obj", centerPlatform + Vector3(0,1,0) );
	mPlatform->SetShrinkValue(0.0f);

	/* set so we cant tilt it more than these angles. */
	mPlatform->SetMaxAngleX(10.0f*(PI/180.0f));
	mPlatform->SetMaxAngleZ(10.0f*(PI/180.0f));
	mPlatform->SetRotate(false);
	mPlatform->SetTargetAngleX(0.5f);
	mPlatform->SetTargetAngleZ(-0.5f);

	PowerBall* mBalls = new PowerBall("Media/Ball.obj", Vector3(-13.0f,27,-13));
	mBalls->SetForwardVector(Vector3(0,0,1));
	mBalls->SetKnockoutMode();
	mBalls->SetAcceleration(mBalls->GetAcceleration()*30.0f);

	GetGraphics()->LoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f, 1.0f, 1.0f);

	mGe->GetCamera()->SetPosition(centerPlatform+ Vector3(0.0f, 30.0f, 20.0f));
	mGe->GetCamera()->LookAt(centerPlatform);

	// Score / results:
	float time = 0.0f;
	bool started = false;

	float delayTimer = 1000.0f;

	iText* timeTxt = GetGraphics()->CreateText("", Vector2(50, 60), 1.0f, "Media/fonts/new");

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
			mBalls = new PowerBall("Media/Ball.obj", Vector3(-13.0f,27,-13));
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
		Vector3 tabort = mBalls->GetPosition();
		if(started)
			time += diff * 0.001f;
		else
		{
			if((mBalls->GetPosition() - Vector3(-13.0f,25,-13)).GetLength() > 3)
			{
				started = true;
			}
		}

		// print score and time text
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
	delete mBox;

	mGe->DeleteText(timeTxt);
}
