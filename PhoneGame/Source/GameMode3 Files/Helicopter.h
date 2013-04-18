#ifndef HELICOPTER
#define HELICOPTER

#include "..\Graphics.h"

class Helicopter
{
public:
	Helicopter(iMesh* chopper, iMesh* rotor, iMesh* secrotor, iTerrain* terr);
	virtual ~Helicopter();

	Vector3 GetPos() { return this->pos; }
	void SetPos(Vector3 pos) { this->pos = pos; this->chopper->SetPosition(this->pos); this->rotor->SetPosition(this->pos); }

	Vector3 GetDircetion() { return this->direction; }
	void SetDirection(Vector3 direction) { this->direction = direction; }

	float GetMass() { return this->mass; }
	void SetMass(float mass) { this->mass = mass; }
	
	float GetElacticity() { return this->elacticity; }
	void SetElacticity(float elacticity) { this->elacticity = elacticity; }

	float GetVelocity() { return Vector3(this->direction).GetLength(); }
	float GetXZVelocity() { return Vector2(this->direction.x, this->direction.y).GetLength(); }
	//void SetVelocity(float velocity) { this->velocity = velocity; }

	void Accelerate(bool acc) { this->accelerating = acc; }
	void Decelerate(bool dec) { this->decelerating = dec; }

	void YawRight(bool yaw) { this->yawingRight = yaw; }
	void YawLeft(bool yaw) { this->yawingLeft = yaw; }

	void RollRight(bool roll) { this->rollingRight = roll; }
	void RollLeft(bool roll) { this->rollingLeft = roll; }
	void RollForward(bool roll) { this->rollingForward = roll; }
	void RollBackward(bool roll) { this->rollingBackward = roll; }

	void Update(float dt);
	void UpdateChopperSpec(float dt);
	void AttuneSecRotorToMainRotor(float dt);

	Vector3 GetForwardVector() { return this->forward; }

private:
	iTerrain* terrain;
	iMesh* chopper;
	iMesh* rotor;
	iMesh* secrotor;
	//Text* rpmtext;
	Vector3 pos;
	Vector3 direction;
	float mass;
	float elacticity;


	bool accelerating;
	bool decelerating;
	bool yawingRight;
	bool yawingLeft;
	bool rollingForward;
	bool rollingBackward;
	bool rollingRight;
	bool rollingLeft;


	float rotorRPM;
	float secRotorRPM;
	Vector3 up;
	Vector3 forward;

};


#endif