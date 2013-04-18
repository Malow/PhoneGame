#include "PhysMesh.h"

#define GRAVITY 9.82f
#define AIR_FRICTION_CONSTANT 0.1f
#define GROUND_FRICTION_CONSTANT 0.3f


PhysMesh::PhysMesh(iMesh* mesh, iTerrain* terr)
{
	this->terrain = terr;
	this->mesh = mesh;
	this->pos = mesh->GetPosition();

	this->direction = Vector3(0, 0, 0);
	this->mass = 1;
	this->elacticity = 1;
}

PhysMesh::~PhysMesh()
{
	if(this->mesh)
		GetGraphics()->DeleteMesh(this->mesh);
}

void PhysMesh::Update(float dt)
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


	this->pos += this->direction * dt;
	this->mesh->SetPosition(this->pos);
}