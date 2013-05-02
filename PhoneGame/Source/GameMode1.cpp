#include "Game.h"

#define SPEED_MULTIPLIER 0.0005f
#define SPEED_CHANGE_MUTLIPLIER 0.01f
#define MINIMUM_SPEED 10.0f
#define MAXIMUM_SPEED 100.0f


#define STAR_POS_COUNT 10
static const Vector3 starPos[STAR_POS_COUNT] = 
{
	Vector3(-16, 17, -9),
	Vector3(-26, 10, -50),
	Vector3(12, 8, 28),
	Vector3(-5, -45, 14),
	Vector3(11, -23, 31),
	Vector3(-8, 45, 41),
	Vector3(41, -14, -23),
	Vector3(3, -48, -46),
	Vector3(-29, 32, 42),
	Vector3(45, -32, -34)
};


void Game::PlayGameMode1()
{
	// Score / results:
	float time = 0.0f;
	int score = 0;
	bool started = false;

	GetGraphics()->ShowLoadingScreen("Media/LoadingScreen/LoadingScreenBG1.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f);
	GetGraphics()->ChangeSkyBox("Media/StarMap.dds"); 
	GetGraphics()->ChangeCamera(RTS);
	GetGraphics()->GetCamera()->SetPosition(Vector3(25, 25, 20));
	GetGraphics()->GetCamera()->LookAt(Vector3(0, 0, 0));

	GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1), Vector3(1, 1, 1), 1.5f);
	GetGraphics()->SetSceneAmbientLight(Vector3(0.4f, 0.4f, 0.4f));
	GetGraphics()->ChangeShadowQuality(0);
	GetGraphics()->UseShadow(false);
	iMesh* model = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(15, 20, 20));
	model->Scale(1.0f * 0.05f);

	iMesh* arrow = GetGraphics()->CreateMesh("Media/RedArrow.obj", Vector3(10, 2000, 15));

	Vector3 ourDir = Vector3(1, 0, 0);
	Vector3 ourUp = Vector3(0, 1, 0);
	float targetSpeed = 50.0f;
	float speed = 50.0f;

	Vector3 defaultDir = Vector3(1, 0, 0);
	Vector3 phoneDir = Vector3(0.9f, 0, 0.2f);
	phoneDir.Normalize();

	iMesh* star = GetGraphics()->CreateMesh("Media/Star1.obj", Vector3(50, 20, 50));
	iBillboard* planetSun = GetGraphics()->CreateBillboard(Vector3(400, 0, 0), Vector2(100.0f, 100.0f), Vector3(1, 1, 1), "Media/planet_sun.png");
	iBillboard* planetMercury = GetGraphics()->CreateBillboard(Vector3(-400, 0, 0), Vector2(50.0f, 50.0f), Vector3(1, 1, 1), "Media/planet_mercury.png");
	iBillboard* planetVenus = GetGraphics()->CreateBillboard(Vector3(0, 0, 400), Vector2(25.0f, 25.0f), Vector3(1, 1, 1), "Media/planet_venus.png");
	iBillboard* planetJupiter = GetGraphics()->CreateBillboard(Vector3(0, 0, -400), Vector2(75.0f, 75.0f), Vector3(1, 1, 1), "Media/planet_jupiter.png");
	iBillboard* planetNeptune = GetGraphics()->CreateBillboard(Vector3(0, 400, 0), Vector2(40.0f, 40.0f), Vector3(1, 1, 1), "Media/planet_neptune.png");
	iBillboard* planetNebula = GetGraphics()->CreateBillboard(Vector3(0, -400, 0), Vector2(60.0f, 60.0f), Vector3(1, 1, 1), "Media/planet_nebula.png");

	iText* targetSpeedTxt = GetGraphics()->CreateText("", Vector2(50, 5), 1.0f, "Media/fonts/new");
	iText* speedTxt = GetGraphics()->CreateText("", Vector2(50, 35), 1.0f, "Media/fonts/new");
	iText* timeTxt = GetGraphics()->CreateText("", Vector2(50, 65), 1.0f, "Media/fonts/new");
	iText* scoreTxt = GetGraphics()->CreateText("", Vector2(50, 95), 1.0f, "Media/fonts/new");

#ifdef _DEBUG
	iText* phoneDirTxtX = GetGraphics()->CreateText("", Vector2(50, 155), 1.0f, "Media/fonts/new");
	iText* phoneDirTxtY = GetGraphics()->CreateText("", Vector2(50, 185), 1.0f, "Media/fonts/new");
	iText* phoneDirTxtZ = GetGraphics()->CreateText("", Vector2(50, 215), 1.0f, "Media/fonts/new");
#endif

	iImage* guiCockpit = GetGraphics()->CreateImage(Vector2(0, GetGraphics()->GetEngineParameters().WindowHeight * 0.12f), Vector2(GetGraphics()->GetEngineParameters().WindowWidth, GetGraphics()->GetEngineParameters().WindowHeight), "Media/cockpit.png");
	iImage* guiStar = GetGraphics()->CreateImage(Vector2(200, 90), Vector2(75, 75), "Media/star.png");
	guiStar->SetOpacity(0.0f);
	float starTimer = 0.0f;

	GetGraphics()->LoadingScreen("Media/LoadingScreen/LoadingScreenBG1.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f, 1.0f, 1.0f);

	go = true;
	int starcolor = 2;
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


		// 3D / game related.
		// Handle Keyboard inputs.
		if(GetGraphics()->GetKeyListener()->IsPressed('W'))
		{
			Vector3 cross = ourDir.GetCrossProduct(ourUp);
			ourDir.RotateAroundAxis(cross, -diff * 0.001f);
			//RotateVectorAroundVector(ourDir, cross, -diff * 0.001f);
			ourUp = cross.GetCrossProduct(ourDir);
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('A'))
		{
			ourUp.RotateAroundAxis(ourDir, diff * 0.001f);
			//RotateVectorAroundVector(ourUp, ourDir, diff * 0.001f);
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('S'))	
		{
			Vector3 cross = ourDir.GetCrossProduct(ourUp);
			ourDir.RotateAroundAxis(cross, diff * 0.001f);
			//RotateVectorAroundVector(ourDir, cross, diff * 0.001f);
			ourUp = cross.GetCrossProduct(ourDir);
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('D'))	
		{
			ourUp.RotateAroundAxis(ourDir, -diff * 0.001f);
			//RotateVectorAroundVector(ourUp, ourDir, -diff * 0.001f);
		}
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_ADD) || GetGraphics()->GetKeyListener()->IsPressed(VK_SHIFT))
			speed += diff * SPEED_CHANGE_MUTLIPLIER;
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_SUBTRACT) || GetGraphics()->GetKeyListener()->IsPressed(VK_CONTROL))
			speed -= diff * SPEED_CHANGE_MUTLIPLIER;


		// print speed text.
		speedTxt->SetText(string("SPEED: " + MaloW::convertNrToString((int)speed)).c_str());
	

		// Handle phone inputs
		if(this->networkController)
		{
			phoneDir = this->networkController->direction;
			phoneDir.Normalize();
			if(phoneDir == Vector3(0, 0, 0))
				phoneDir = Vector3(1, 0, 0);

			//////////////////////////////////////////////////////////////////////////
			// Make it so that u dont have to turn the phone completely for it to give full effect:
			// Add algortihm for increasing Z when Y is high, and the other way around, this because when Y is increased Z 
			//      will be decreased because it's a normalized vector.. Maybe use X to play around with and have 
			//				both multiplied by X or something? Also need to make movements around small numbers less noticable (think spotify volume level)

			// Divide by X. The higher Z and Y is the smaller X will be = the bigger Z and Y will be. Should mean that if u keep Z steady turned but turn Y more, both Z and X should get 
			// redcued due to normalized vector, and then Z and Y gets increased due to divided by a smaller X.
			phoneDir.z /= abs(phoneDir.x);
			phoneDir.y /= abs(phoneDir.x);

			// The smaller the value is the smaller the turn will be, should give a nice non-linear acceleration like spotify. 
			// right now 0.0 - 0.1 is increased non-linearly, and then linearly after that.
			
			if(abs(phoneDir.y) < 0.33f)
				phoneDir.y *= abs(phoneDir.y) * 3;

			if(abs(phoneDir.z) < 0.33f)
				phoneDir.z *= abs(phoneDir.z) * 3;
				
			// Double the value to increase the sens so u dont have to turn the phone completely for full effect.
			/*
			phoneDir.y *= 1.5f;
			phoneDir.z *= 1.5f;
			*/

			// Limit values to -1.0 - 1.0
			if(phoneDir.z > 1.0f)
				phoneDir.z = 1.0f;
			if(phoneDir.z < -1.0f)
				phoneDir.z = -1.0f;
			if(phoneDir.y > 1.0f)
				phoneDir.y = 1.0f;
			if(phoneDir.y < -1.0f)
				phoneDir.y = -1.0f;

#ifdef _DEBUG
			phoneDirTxtX->SetText(string("PHONEDIR: X: " + MaloW::convertNrToString(phoneDir.x)).c_str());
			phoneDirTxtY->SetText(string("PHONEDIR: Y: " + MaloW::convertNrToString(phoneDir.y)).c_str());
			phoneDirTxtZ->SetText(string("PHONEDIR: Z: " + MaloW::convertNrToString(phoneDir.z)).c_str());
#endif
			//////////////////////////////////////////////////////////////////////////

			// min / max: 10 / 100: speed: 10 -> min(10) + input(10) * range(0.9) = 19
			targetSpeed = MINIMUM_SPEED + this->networkController->speed * ((MAXIMUM_SPEED - MINIMUM_SPEED) * 0.01f);
			
			if(targetSpeed < MINIMUM_SPEED)
				targetSpeed = MINIMUM_SPEED;
			if(targetSpeed > MAXIMUM_SPEED)
				targetSpeed = MAXIMUM_SPEED;

			targetSpeedTxt->SetText(string("TARGETSPEED: " + MaloW::convertNrToString((int)targetSpeed)).c_str());

			Vector3 cross = ourDir.GetCrossProduct(ourUp);
			ourDir.RotateAroundAxis(cross, -phoneDir.z * diff * 0.001f);
			//RotateVectorAroundVector(ourDir, cross, -phoneDir.z * diff * 0.001f);
			ourUp = cross.GetCrossProduct(ourDir);
			ourUp.RotateAroundAxis(ourDir, -phoneDir.y * diff * 0.001f);
			//RotateVectorAroundVector(ourUp, ourDir, -phoneDir.y * diff * 0.001f);

			// Apply phone's targetspeed to speed
			if(targetSpeed > speed)
			{
				speed += diff * SPEED_CHANGE_MUTLIPLIER;
				if(speed > targetSpeed)
					speed = targetSpeed;
			}
			if(targetSpeed < speed)
			{
				speed -= diff * SPEED_CHANGE_MUTLIPLIER;
				if(speed < targetSpeed)
					speed = targetSpeed;
			}
		}

		if(speed < MINIMUM_SPEED)
			speed = MINIMUM_SPEED;

		if(speed > MAXIMUM_SPEED)
			speed = MAXIMUM_SPEED;


		// Handle game objectives
		if(started)
			time += diff * 0.001f;
		if((GetGraphics()->GetCamera()->GetPosition() - star->GetPosition()).GetLength() < 3.0f)
		{
			GetGraphics()->DeleteMesh(star);
			star = GetGraphics()->CreateMesh(string("Media/Star" + MaloW::convertNrToString(starcolor++) + ".obj").c_str(), starPos[score%STAR_POS_COUNT]);

			if(starcolor > 2)
				starcolor = 1;

			if(score == 0)
				started = true;
			score++;

			starTimer = 2.0f;

			// Do Vibration
			if(this->networkController)
			{
				this->networkController->cc->sendData("VIB: 300");
			}
		}
		// print score and time text.
		scoreTxt->SetText(string("SCORE: " + MaloW::convertNrToString(score)).c_str());
		timeTxt->SetText(string("TIME: " + MaloW::convertNrToString((int)time)).c_str());
		starTimer -= diff * 0.001f;
		if(starTimer < 0.0f)
			starTimer = 0.0f;
		guiStar->SetOpacity(starTimer);
		star->Rotate(Vector3(diff, diff, diff) * 0.002f);

		// Update arrow pointing towards next game objective
		Vector3 VecToObjective =  star->GetPosition() - GetGraphics()->GetCamera()->GetPosition();
		GetGraphics()->GetCamera()->SetMesh(arrow, Vector3(ourDir * -2 - ourUp));
		//arrow->SetPosition(GetGraphics()->GetCamera()->GetPosition() + ourDir * 2 + ourUp);
		arrow->ResetRotation();
		Vector3 vec = Vector3(0, -1, 0);
		Vector3 around = vec.GetCrossProduct(VecToObjective);
		float angle = -acos(vec.GetDotProduct(VecToObjective) / (vec.GetLength() * VecToObjective.GetLength()));
		arrow->RotateAxis(around, -angle);
		arrow->SetScale(log(1 + VecToObjective.GetLength()) * 0.03f);


		// Fix the camera to our vectors.
		ourDir.Normalize();
		ourUp.Normalize();
		GetGraphics()->GetCamera()->Move(ourDir * diff * SPEED_MULTIPLIER * speed);
		GetGraphics()->GetCamera()->SetForward(ourDir);
		GetGraphics()->GetCamera()->SetUpVector(ourUp);

		// End game after 2 mins
		if(time > 120.0f)
		{
			this->FinishScreen(score, "1, Starchaser", time);
			go = false;
		}
	}

	if(this->networkController)
	{
		this->networkController->cc->sendData("QUITTING");
	}

	GetGraphics()->GetCamera()->RemoveMesh();

	// Delete all stuff
	GetGraphics()->DeleteMesh(model);
	GetGraphics()->DeleteMesh(arrow);
	GetGraphics()->DeleteMesh(star);

	GetGraphics()->DeleteBillboard(planetSun);
	GetGraphics()->DeleteBillboard(planetMercury);
	GetGraphics()->DeleteBillboard(planetVenus);
	GetGraphics()->DeleteBillboard(planetJupiter);
	GetGraphics()->DeleteBillboard(planetNeptune);
	GetGraphics()->DeleteBillboard(planetNebula);

	GetGraphics()->DeleteText(targetSpeedTxt);
	GetGraphics()->DeleteText(speedTxt);
	GetGraphics()->DeleteText(timeTxt);
	GetGraphics()->DeleteText(scoreTxt);

#ifdef _DEBUG
	GetGraphics()->DeleteText(phoneDirTxtX);
	GetGraphics()->DeleteText(phoneDirTxtY);
	GetGraphics()->DeleteText(phoneDirTxtZ);
#endif

	GetGraphics()->DeleteImage(guiCockpit);
	GetGraphics()->DeleteImage(guiStar);
}

// TODO:
// Turning less effective with lower speed -> Test before doing this.