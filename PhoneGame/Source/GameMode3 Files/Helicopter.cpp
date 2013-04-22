#include "Helicopter.h"

#define GRAVITY 9.82f
#define AIR_DENSITY 1.2f

// Increase air friction for better gameplay
//#define AIR_FRICTION_CONSTANT 0.1f
#define AIR_FRICTION_CONSTANT 5.0f

// Increase to not be able to glide on the ground.
//#define GROUND_FRICTION_CONSTANT 0.3f
#define GROUND_FRICTION_CONSTANT 5.0f

// Taken from http://www.defensie.nl/english/subjects/materiel/aircraft_and_helicopters/helicopters/apache_ah-64d_longbow_attack_helicopter
// Not anymore
#define MAX_ROTOR_RPM 15.0f
#define MAX_SEC_ROTOR_RPM (MAX_ROTOR_RPM * 4.0f)


#define ROLLING_COEF 0.2f
#define YAWING_COEF 0.0001f
#define PHONE_INPUT_COEF 0.001f


Helicopter::Helicopter(iMesh* chopper, iMesh* rotor, iMesh* secrotor, iTerrain* terr)
{
	this->terrain = terr;
	this->chopper = chopper;
	this->rotor = rotor;
	this->secrotor = secrotor;

	this->pos = chopper->GetPosition();

	this->direction = Vector3(0, 0, 0);
	this->mass = 1000;
	this->elacticity = 0.1;
	this->rotorRPM = 0;
	this->accelerating = false;
	this->decelerating = false;

	this->up = Vector3(0, 1, 0);
	this->forward = Vector3(1, 0, 0);

	bool yawingRight = false;
	bool yawingLeft = false;
	this->secRotorRPM = 0;
	this->engineStarted = false;
	this->camOffsetHeight = 20.0f;
	this->camOffsetDistance = 40.0f;
	//this->rpmtext = GetGraphicsEngine()->CreateText("RPM: " + MaloW::convertNrToString(this->rotorRPM), D3DXVECTOR2(50, 50), 1.0f, "Media/Fonts/1");
}

Helicopter::~Helicopter()
{
	//if(this->rpmtext)
		//GetGraphicsEngine()->DeleteText(this->rpmtext);
}

void Helicopter::Update(float dt)
{
	// Gravity
	this->direction.y -= GRAVITY * dt;


	// Air friction
	float velocity = this->GetVelocity();
	float newVelocity = velocity - ((AIR_FRICTION_CONSTANT * (velocity * velocity)) / this->mass) * dt;
	float dv = newVelocity / velocity;
	this->direction *= dv;
	

	// Collision against terrain
	float terrY = this->terrain->GetYPositionAt(this->pos.x, this->pos.z);
	if(this->pos.y + this->direction.y * dt <= terrY)
	{
		// Bounce
		this->direction.y = 0 - this->direction.y * this->elacticity;
	}
	
	
	// Friction against terrain here if close to it, simplified calculating only in the XZ plane and not angled terrain.
	if(this->pos.y - 0.5f < terrY)
	{
		float frictionForce = GROUND_FRICTION_CONSTANT * this->mass * GRAVITY;
		float frictionAcc = (frictionForce / this->mass) * dt;
		if(frictionAcc > this->GetXZVelocity())
		{
			this->direction.x = 0;
			this->direction.z = 0;
		}
		else
		{
			velocity = this->GetVelocity();
			newVelocity = velocity - frictionAcc;
			dv = newVelocity / velocity;
			this->direction *= dv;
		}

		// Reset rolling when touching ground.
		Vector3 defup = Vector3(0, 1, 0);
		defup.Normalize();
		Vector3 cross = this->up.GetCrossProduct(this->forward);
		cross.Normalize();
		float dotCrossPhone = cross.GetDotProduct(defup);
		// Roll left
		if(dotCrossPhone < 0.0f)
		{
			this->RollLeft(true);
			this->RollRight(false);
		}
		// Roll right
		else if(dotCrossPhone > 0.0f)
		{
			this->RollLeft(false);
			this->RollRight(true);
		}

		float dotForPhone = this->forward.GetDotProduct(defup);
		// Roll forward
		if(dotForPhone > 0.0f)
		{
			this->RollForward(true);
			this->RollBackward(false);
		}
		// Roll backward
		else if(dotForPhone < 0.0f)
		{
			this->RollForward(false);
			this->RollBackward(true);
		}
	}

	this->UpdateChopperSpec(dt);


	// For funness we need to reset the direction.y a little, it's too hard to handle up&downs otherwise
	//direction.y *= 1.0f - dt;

	// AutoHoverEffect
	this->AutoHover(dt);


	this->pos += this->direction * dt;
	this->chopper->SetPosition(this->pos);
	this->rotor->SetPosition(this->pos);
	this->secrotor->SetPosition(this->pos - this->forward * 28 + this->up * 11);	// Adding offset for the second rotor.

	Vector3 lookAt = this->forward;
	lookAt.y = 0.0f;
	GetGraphics()->GetCamera()->SetPosition(this->pos + Vector3(0, this->camOffsetHeight, 0) - lookAt * this->camOffsetDistance);
	GetGraphics()->GetCamera()->LookAt(this->pos + lookAt * 30 + Vector3(0, 10, 0));

	
	// Attune forward to direction.
	Vector3 xzdir = this->direction;
	xzdir.y = 0.0f;
	float xzlen = xzdir.GetLength();
	xzdir.Normalize();
	Vector3 xzfor = this->forward;
	xzfor.y = 0.0f;
	xzfor.Normalize();
	float dotDirFor = xzdir.GetDotProduct(xzfor);
	Vector3 vecRight = xzfor.GetCrossProduct(Vector3(0, 1, 0));
	vecRight.y = 0.0f;
	vecRight.Normalize();
	float dotDirRight = xzdir.GetDotProduct(vecRight);

	// When flying backwards remove the "BAM SNAP" effect when dot product gets 0.0f at the sides.
	if(dotDirFor < 0.0f)
		dotDirFor = 0.0f;

	float angle = dt * xzlen * xzlen * xzlen * xzlen * (1 - abs(dotDirFor)) * 0.000001f;
	if(dotDirRight < 0.0f)
		angle *= -1.0f;
	this->forward.RotateAroundAxis(this->up, -angle);
	this->chopper->RotateAxis(this->up, -angle);
	this->rotor->RotateAxis(this->up, angle);
}

void Helicopter::UpdateChopperSpec(float dt)
{
	// Acc and Dec
	if(this->accelerating)
	{
		this->engineStarted = true;
		this->rotorRPM += dt;
		if(this->rotorRPM > MAX_ROTOR_RPM)
			this->rotorRPM = MAX_ROTOR_RPM;
		else 
			this->secRotorRPM += dt * 2;
	}
	else if(this->decelerating)
	{
		this->rotorRPM -= 2 * dt;		// 2 times quicker deceleration
		if(this->rotorRPM < 0)
			this->rotorRPM = 0.0f;
		else
			this->secRotorRPM -= dt * 4;

		if(this->rotorRPM == 0.0f)
			this->engineStarted = false;
	}
	else
	{
		// Attunation of rotorspeed to make it auto-hover.
		if(this->rotorRPM > 11.4f)
		{
			this->rotorRPM -= dt * 0.1f;
		}
	}
	
	// Apply yawing
	if(this->rotorRPM / MAX_ROTOR_RPM > 0.3f)	// If spinning enough
	{
		// Yaw left and right
		if(this->yawingRight)
		{
			this->secRotorRPM += dt * 2;
		}
		if(this->yawingLeft)
		{
			this->secRotorRPM -= dt * 2;
		}
		this->AttuneSecRotorToMainRotor(dt);
	
		float angle = (this->rotorRPM * (MAX_SEC_ROTOR_RPM / MAX_ROTOR_RPM) / 2 - this->secRotorRPM) * YAWING_COEF;
		this->forward.RotateAroundAxis(this->up, -angle);
		this->chopper->RotateAxis(this->up, -angle);
		this->rotor->RotateAxis(this->up, angle);
	}


	// Apply Rolling
	if(this->rotorRPM / MAX_ROTOR_RPM > 0.3f)	// If spinning enough
	{
		if(this->rollingForward)
		{
			Vector3 vec(this->up);
			Vector3 around = vec.GetCrossProduct(Vector3(this->forward));
			float angle = -dt * ROLLING_COEF;
			this->forward.RotateAroundAxis(around, -angle);
			this->up.RotateAroundAxis(around, -angle);
			this->chopper->RotateAxis(around, -angle);
			this->rotor->RotateAxis(around, -angle);
		}
		if(this->rollingBackward)
		{
			Vector3 vec(this->up);
			Vector3 around = vec.GetCrossProduct(Vector3(this->forward));
			float angle = dt * ROLLING_COEF;
			this->forward.RotateAroundAxis(around, -angle);
			this->up.RotateAroundAxis(around, -angle);
			this->chopper->RotateAxis(around, -angle);
			this->rotor->RotateAxis(around, -angle);
		}
		if(this->rollingLeft)
		{
			float angle = -dt * ROLLING_COEF;
			this->up.RotateAroundAxis(this->forward, -angle);
			this->chopper->RotateAxis(this->forward, -angle);
			this->rotor->RotateAxis(this->forward, -angle);
		}
		if(this->rollingRight)
		{
			float angle = dt * ROLLING_COEF;
			this->up.RotateAroundAxis(this->forward, -angle);
			this->chopper->RotateAxis(this->forward, -angle);
			this->rotor->RotateAxis(this->forward, -angle);
		}
	}

	// Apply lifting force
	float C = 10.0f;
	float S = 0.8f;
	// w = v^2 / r
	float bladeVelocity = sqrt((this->rotorRPM * 60.0f) * 1.5f);
	float liftForce = C * (0.5f * AIR_DENSITY * (bladeVelocity * bladeVelocity) * S);
	float liftAcc = liftForce / this->mass;
	this->direction += this->up * liftAcc * dt;
	
	//Apply lifting force by air-friction
	float frictionForce = C * (0.5f * AIR_DENSITY * (bladeVelocity * bladeVelocity) * S);
	float frictionAcc = frictionForce / this->mass;
	this->direction += this->up * frictionAcc * dt;
	
	this->rotor->RotateAxis(this->up, dt * this->rotorRPM);
	Vector3 vec(this->up);
	Vector3 around = vec.GetCrossProduct(Vector3(this->forward));
	this->secrotor->RotateAxis(around, dt * this->secRotorRPM);
}

void Helicopter::AttuneSecRotorToMainRotor(float dt)
{
	// Attune sec to keep the sec-rotor rpm linear with the main rotor to simulate balancing spin.
	if((this->secRotorRPM / MAX_SEC_ROTOR_RPM) * 2 < (this->rotorRPM / MAX_ROTOR_RPM))
	{
		this->secRotorRPM += dt * 0.5f;
	}
	if((this->secRotorRPM / MAX_SEC_ROTOR_RPM) * 2 > (this->rotorRPM / MAX_ROTOR_RPM))
	{
		this->secRotorRPM -= dt * 0.5f;
	}
	

	// Enforce max and min spin
	if(this->secRotorRPM > MAX_SEC_ROTOR_RPM)
		this->secRotorRPM = MAX_SEC_ROTOR_RPM;

	if(this->secRotorRPM < 0)
		this->secRotorRPM = 0;
}

void Helicopter::SetUpVector(Vector3 nUp)
{
	nUp.Normalize();
	Vector3 cross = this->up.GetCrossProduct(this->forward);
	cross.Normalize();
	float dotCrossnUp = cross.GetDotProduct(nUp);

	// Roll left
	if(dotCrossnUp < 0.0f)
	{
		float angle = dotCrossnUp * 3.1415f * 0.5f;
		this->up.RotateAroundAxis(this->forward, -angle);
		this->chopper->RotateAxis(this->forward, -angle);
		this->rotor->RotateAxis(this->forward, -angle);
	}
	// Roll right
	else
	{
		float angle = dotCrossnUp * 3.1415f * 0.5f;
		this->up.RotateAroundAxis(this->forward, -angle);
		this->chopper->RotateAxis(this->forward, -angle);
		this->rotor->RotateAxis(this->forward, -angle);
	}
	
	float dotFornUp = this->forward.GetDotProduct(nUp);
	// Roll forward
	if(dotFornUp < 0.0f)
	{
		Vector3 vec(this->up);
		Vector3 around = vec.GetCrossProduct(Vector3(this->forward));
		float angle = dotFornUp * 3.1415f * 0.5f;
		this->forward.RotateAroundAxis(around, -angle);
		this->up.RotateAroundAxis(around, -angle);
		this->chopper->RotateAxis(around, -angle);
		this->rotor->RotateAxis(around, -angle);
	}
	// Roll backward
	else
	{
		Vector3 vec(this->up);
		Vector3 around = vec.GetCrossProduct(Vector3(this->forward));
		float angle = dotFornUp * 3.1415f * 0.5f;
		this->forward.RotateAroundAxis(around, -angle);
		this->up.RotateAroundAxis(around, -angle);
		this->chopper->RotateAxis(around, -angle);
		this->rotor->RotateAxis(around, -angle);
	}
}

Vector3 Helicopter::GetRightVector()
{
	Vector3 right = this->up.GetCrossProduct(this->forward);
	return right;
}

void Helicopter::PhoneInput(Vector3 phoneDir, float dt)
{
	phoneDir.Normalize();
	Vector3 cross = this->up.GetCrossProduct(this->forward);
	cross.Normalize();
	float dotCrossPhone = cross.GetDotProduct(phoneDir);

	// Roll left
	if(dotCrossPhone < 0.0f)
	{
		float angle = dotCrossPhone * dt * PHONE_INPUT_COEF;
		this->up.RotateAroundAxis(this->forward, -angle);
		this->chopper->RotateAxis(this->forward, -angle);
		this->rotor->RotateAxis(this->forward, -angle);
	}
	// Roll right
	else
	{
		float angle = dotCrossPhone * dt * PHONE_INPUT_COEF;
		this->up.RotateAroundAxis(this->forward, -angle);
		this->chopper->RotateAxis(this->forward, -angle);
		this->rotor->RotateAxis(this->forward, -angle);
	}
	/*
	float dotForPhone = this->forward.GetDotProduct(phoneDir);
	// Roll forward
	if(dotForPhone > 0.0f)
	{
		Vector3 around = this->up.GetCrossProduct(this->forward);
		float angle = dotForPhone * dt * PHONE_INPUT_COEF;
		this->forward.RotateAroundAxis(around, -angle);
		this->up.RotateAroundAxis(around, -angle);

		this->up = around.GetCrossProduct(this->forward);

		this->chopper->RotateAxis(around, -angle);
		this->rotor->RotateAxis(around, -angle);
	}
	// Roll backward
	else if(dotForPhone < 0.0f)
	{
		Vector3 around = this->up.GetCrossProduct(this->forward);
		float angle = dotForPhone * dt * PHONE_INPUT_COEF;
		this->forward.RotateAroundAxis(around, -angle);
		this->up.RotateAroundAxis(around, -angle);

		this->up = around.GetCrossProduct(this->forward);

		this->chopper->RotateAxis(around, -angle);
		this->rotor->RotateAxis(around, -angle);
	}*/
}

float Helicopter::GetMaxRPM()
{
	return MAX_ROTOR_RPM;
}

float Helicopter::GetCurrentRPM()
{
	return this->rotorRPM;
}

void Helicopter::AutoHover( float dt )
{
	float hoverRPM = 0.0f;	// Find out using below:

	if(this->up.y == 0.0f)
		return;

	float C = 10.0f;
	float S = 0.8f;
	hoverRPM = 9.82f * 0.5f * (1.0f / this->up.y) * this->mass * (1.0f / C) * (1.0f / S) * 2.0f * (1.0f / AIR_DENSITY) * (1.0f / 1.5f) * (1.0f / 60.0f);

	// Attune rotorRPM to hoverRPM:
	if(this->engineStarted)
	{
		if(hoverRPM > this->rotorRPM && !this->decelerating && !this->accelerating)
		{
			this->rotorRPM += dt;
			if(this->rotorRPM > MAX_ROTOR_RPM)
				this->rotorRPM = MAX_ROTOR_RPM;
			else 
				this->secRotorRPM += dt * 2;
		}
		if(hoverRPM < this->rotorRPM && !this->accelerating && !this->decelerating)
		{
			this->rotorRPM -= dt;
			this->secRotorRPM -= dt * 2;
		}
	}
}


