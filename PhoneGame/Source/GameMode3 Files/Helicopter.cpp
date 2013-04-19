#include "Helicopter.h"

#define GRAVITY 9.82f
#define AIR_DENSITY 1.2f

// Increase air friction for better gameplay
//#define AIR_FRICTION_CONSTANT 0.1f
#define AIR_FRICTION_CONSTANT 5.0f

#define GROUND_FRICTION_CONSTANT 0.3f

// Taken from http://www.defensie.nl/english/subjects/materiel/aircraft_and_helicopters/helicopters/apache_ah-64d_longbow_attack_helicopter
// Not anymore
#define MAX_ROTOR_RPM 15.0f
#define MAX_SEC_ROTOR_RPM (MAX_ROTOR_RPM * 4.0f)


#define ROLLING_COEF 0.2f
#define YAWING_COEF 0.0001f


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
	if(this->pos.y - 0.7 < terrY)
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
	}

	this->UpdateChopperSpec(dt);
	
	this->pos += this->direction * dt;
	this->chopper->SetPosition(this->pos);
	this->rotor->SetPosition(this->pos);
	this->secrotor->SetPosition(this->pos - this->forward * 28 + this->up * 11);	// Adding offset for the second rotor.

	Vector3 lookAt = this->forward;
	lookAt.y = 0.0f;
	GetGraphics()->GetCamera()->SetPosition(this->pos + Vector3(0, 20, 0) - lookAt * 40);
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

	// For funness we need to reset the direction.y a little, it's too hard to handle up&downs otherwise
}

void Helicopter::UpdateChopperSpec(float dt)
{
	// Acc and Dec
	if(this->accelerating)
	{
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
			this->rotorRPM = 0;
		else
			this->secRotorRPM -= dt * 4;
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