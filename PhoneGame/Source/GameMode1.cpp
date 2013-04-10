#include "Game.h"

#define SPEED_MULTIPLIER 0.0005f

float t(float angle)
{
	return 1-(float)cos((double)angle);
}

float c(float angle)
{
	return (float)cos((double)angle);
}

float s(float angle)
{
	return (float)sin((double)angle);
}

float a1(float angle, Vector3 axis, float tr, float cos)
{
	return (tr * axis.x * axis.x) + cos;
}

float a2(float angle, Vector3 axis, float tr, float sin)
{
	return (tr * axis.x * axis.y) - (sin * axis.z);
}

float a3(float angle, Vector3 axis, float tr, float sin)
{
	return (tr * axis.x * axis.z) + (sin * axis.y);
}

float b1(float angle, Vector3 axis, float tr, float sin)
{
	return (tr * axis.x * axis.y) + (sin * axis.z);
}

float b2(float angle, Vector3 axis, float tr, float cos)
{
	return (tr * axis.y * axis.y) + cos;
}

float b3(float angle, Vector3 axis, float tr, float sin)
{
	return (tr * axis.y * axis.z) - (sin * axis.x);
}

float c1(float angle, Vector3 axis, float tr, float sin)
{
	return (tr * axis.x * axis.z) - (sin * axis.y);
}

float c2(float angle, Vector3 axis, float tr, float sin)
{
	return (tr * axis.y * axis.z) + (sin * axis.x);
}

float c3(float angle, Vector3 axis, float tr, float cos)
{
	return (tr * axis.z * axis.z) + cos;
}

void RotateVectorAroundVector(Vector3& vector, Vector3& axis, float angle)
{
	float tr = t(angle);
	float cos = c(angle);
	float sin = s(angle);

	vector.x = a1(angle, axis, tr, cos) * vector.x + a2(angle, axis, tr, sin) * vector.y + a3(angle, axis, tr, sin) * vector.z;
	vector.y = b1(angle, axis, tr, sin) * vector.x + b2(angle, axis, tr, cos) * vector.y + b3(angle, axis, tr, sin) * vector.z;
	vector.z = c1(angle, axis, tr, sin) * vector.x + c2(angle, axis, tr, sin) * vector.y + c3(angle, axis, tr, cos) * vector.z; 
}

void Game::PlayGameMode1()
{
	// Score / results:
	float time = 0.0f;
	int score = 0;
	bool started = false;



	GetGraphics()->ShowLoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f);
	GetGraphics()->CreateSkyBox("Media/StarMap.dds"); 
	GetGraphics()->GetCamera()->SetPosition(Vector3(25, 25, 20));
	GetGraphics()->GetCamera()->LookAt(Vector3(0, 0, 0));

	GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1), Vector3(1, 1, 1), 1.5f);
	GetGraphics()->SetSceneAmbientLight(Vector3(0.4f, 0.4f, 0.4f));
	iMesh* model = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(15, 20, 20));
	model->Scale(1.0f * 0.05f);

	iMesh* arrow = GetGraphics()->CreateMesh("Media/RedArrow.obj", Vector3(10, 20, 15));


	GetGraphics()->LoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png", 1.0f, 1.0f, 1.0f, 1.0f);

	Vector3 ourDir = Vector3(1, 0, 0);
	Vector3 ourUp = Vector3(0, 1, 0);
	float speed = 50.0f;

	Vector3 defaultDir = Vector3(1, 0, 0);
	Vector3 phoneDir = Vector3(0.9f, 0, 0.2f);
	phoneDir.Normalize();

	iMesh* creep = GetGraphics()->CreateMesh("Media/Creep.obj", Vector3(50, 20, 50));

	iText* speedTxt = GetGraphics()->CreateText("", Vector2(50, 20), 1.0f, "Media/fonts/new");
	iText* timeTxt = GetGraphics()->CreateText("", Vector2(50, 50), 1.0f, "Media/fonts/new");
	iText* scoreTxt = GetGraphics()->CreateText("", Vector2(50, 80), 1.0f, "Media/fonts/new");

	iImage* guiCockpit = GetGraphics()->CreateImage(Vector2(0, GetGraphics()->GetEngineParameters().WindowHeight * 0.1f), Vector2(GetGraphics()->GetEngineParameters().WindowWidth, GetGraphics()->GetEngineParameters().WindowHeight), "Media/cockpit.png");

	bool go = true;
	GetGraphics()->Update();
	while(GetGraphics()->IsRunning() && go)
	{
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
			RotateVectorAroundVector(ourDir, cross, -diff * 0.001f);
			ourUp = cross.GetCrossProduct(ourDir);
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('A'))
		{
			RotateVectorAroundVector(ourUp, ourDir, diff * 0.001f);
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('S'))	
		{
			Vector3 cross = ourDir.GetCrossProduct(ourUp);
			RotateVectorAroundVector(ourDir, cross, diff * 0.001f);
			ourUp = cross.GetCrossProduct(ourDir);
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('D'))	
		{
			RotateVectorAroundVector(ourUp, ourDir, -diff * 0.001f);
		}
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_ADD))
			speed += diff * 0.01f;
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_SUBTRACT))
			speed -= diff * 0.01f;

		// print speed text.
		speedTxt->SetText(string("SPEED: " + MaloW::convertNrToString(speed)).c_str());

		// Handle phone inputs
		if(this->networkController)
		{
			phoneDir = this->networkController->direction;
			phoneDir.Normalize();
			if(phoneDir == Vector3(0, 0, 0))
				phoneDir = Vector3(1, 0, 0);

			speed = this->networkController->speed;
			if(speed == 0.0f)
				speed = 50.0f;
			

			Vector3 cross = ourDir.GetCrossProduct(ourUp);
			RotateVectorAroundVector(ourDir, cross, -phoneDir.z * 0.001f);
			ourUp = cross.GetCrossProduct(ourDir);
			
			RotateVectorAroundVector(ourUp, ourDir, -phoneDir.y * 0.001f);
		}


		// Handle game objectives
		if(started)
			time += diff * 0.001f;
		if((GetGraphics()->GetCamera()->GetPosition() - creep->GetPosition()).GetLength() < 2.5f)
		{
			creep->SetPosition(Vector3(rand()%100 - 50, rand()%100 - 50, rand()%100 - 50));
			if(score == 0)
				started = true;
			score++;
		}
		// print score and time text.
		scoreTxt->SetText(string("SCORE: " + MaloW::convertNrToString(score)).c_str());
		timeTxt->SetText(string("TIME: " + MaloW::convertNrToString(time)).c_str());


		// Update arrow pointing towards next game objective
		Vector3 VecToObjective =  creep->GetPosition() - GetGraphics()->GetCamera()->GetPosition();
		arrow->SetPosition(GetGraphics()->GetCamera()->GetPosition() + ourDir * 2 + ourUp);
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
	}
}



// Colorization of balls randomly.

// Color overlay on the arrow for 1 sec after taking a ball.

// Minimum gas speed

// Turning less effective with lower speed

// Not being able to instant change speed.