#ifndef PHYSMESH
#define PHYSMESH

#include "..\Graphics.h"

class PhysMesh
{
public:
	PhysMesh(iMesh* mesh, iTerrain* terr);
	virtual ~PhysMesh();

	Vector3 GetPos() { return this->pos; }
	void SetPos(Vector3 pos) { this->pos = pos; this->mesh->SetPosition(this->pos); }

	Vector3 GetDircetion() { return this->direction; }
	void SetDirection(Vector3 direction) { this->direction = direction; }

	float GetMass() { return this->mass; }
	void SetMass(float mass) { this->mass = mass; }
	
	float GetElacticity() { return this->elacticity; }
	void SetElacticity(float elacticity) { this->elacticity = elacticity; }

	float GetVelocity() { return Vector3(this->direction).GetLength(); }
	float GetXZVelocity() { return Vector2(this->direction.x, this->direction.y).GetLength(); }
	//void SetVelocity(float velocity) { this->velocity = velocity; }

	void Update(float dt);

private:
	iTerrain* terrain;
	iMesh* mesh;
	Vector3 pos;
	Vector3 direction;
	float mass;
	float elacticity;

};


#endif