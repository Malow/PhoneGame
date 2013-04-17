#include "GameObject.h"
#include "Graphics.h"

GameObject::GameObject()
{
	this->mMesh = NULL;
	this->mStartPosition = Vector3(0,0,0);
}

GameObject::~GameObject()
{
	if(this->mMesh)
		GetGraphics()->DeleteMesh(this->mMesh);
}
Vector3 GameObject::GetPosition() const 
{ 
	return this->mMesh->GetPosition(); 
}

Vector3 GameObject::GetPositionVector3() const
{
	Vector3 pos = this->mMesh->GetPosition();
	Vector3 position = Vector3(pos.x, pos.y, pos.z);
	return position;
}
	
iMesh* GameObject::GetMesh() const
{
	return this->mMesh;
}

void GameObject::SetPosition(const Vector3 position) 
{ 
	this->mMesh->SetPosition(position); 
}
			
void GameObject::SetPosition(const float x, const float y, const float z) 
{ 
	this->mMesh->SetPosition(Vector3(x,y,z)); 
}

void GameObject::SetMesh(iMesh* mesh) 
{
	this->mMesh = mesh;
}