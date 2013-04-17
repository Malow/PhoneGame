#include "Map.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "Graphics.h"

using namespace std;

#define PI 3.1415

Map::Map(string meshFilePath, Vector3 position)
{
	this->mMesh		     = GetGraphics()->CreateStaticMesh(meshFilePath.c_str(), position); 
	this->mShrink	     = 0.03f;
	this->mRadius	     = 100000.0f;
	this->mScaledRadius  = this->mRadius;
	this->mMass			 = 100000.0f;
	this->mRestitution   = 0.60f;
	this->mIsRotating	 = false;
	this->mAngleX		 = 0.0f;
	this->mAngleZ		 = 0.0f;
	this->mMaxAngleX	 = 0.0f;
	this->mMaxAngleZ	 = 0.0f;
	this->mMeshHotZone = NULL;
	this->mAngleY = 0.0f;
	this->mTimeInHotZoneContinuously = 0.0f;
}
Map::~Map()
{
	GetGraphics()->DeleteMesh(this->mMesh);
	if(this->mMeshHotZone)
		GetGraphics()->DeleteMesh(this->mMeshHotZone);
}

Vector3 Map::GetPositionXZ() const
{
	Vector3 temp = this->mMesh->GetPosition();
	Vector3 temp2 = Vector3(temp.x,0,temp.z);
	return temp2;
}
void Map::Update(const float dt)
{
	if(this->mIsRotating)
	{		
		float angle;
		float speed = PI/26.0f;
		if(this->mTargetAngleX <0)
			angle = -speed*dt*0.001f;
		else
			angle = speed*dt*0.001f;
		Matrix3 temp;
		temp.SetRotationZ(this->mAngleZ);
		Vector3 xA = Vector3(1,0,0);
		Vector3 xAnew = temp * xA;
		this->mAngleX += angle;
		this->mMesh->RotateAxis(xAnew, angle);
		
		if(this->mTargetAngleZ <0)
			angle = -speed*dt*0.001f;
		else
			angle = speed*dt*0.001f;
		this->mAngleZ += angle;
		this->mMesh->RotateAxis(Vector3(0,0,1), angle);
	}
	float newdt = dt * 0.001f;
	float fraction = 1.0f - this->mShrink * newdt;
	//this->mMesh->Scale(Vector3(fraction,1,fraction));
	this->mScaledRadius *= fraction;//this->mScaledRadius/this->mRadius;
}
/*void Map::Update(const float dt)
{
	
	float fraction = 1.0f-this->mShrink*dt;
	this->mScaledRadius *= fraction;//this->mScaledRadius/this->mRadius;
	this->mMesh->Scale(D3DXVECTOR3(fraction,1,fraction));
}*/

bool Map::IsTargetAngleReachedX() const
{
	if(this->mTargetAngleX < 0)
	{
		if( this->mAngleX > this->mTargetAngleX )
			return false;
		else
			return true;
	}
	else
	{
		if( this->mAngleX < this->mTargetAngleX)
			return false;
		else
			return true;
	}
}

bool Map::IsTargetAngleReachedZ() const
{
	if(this->mTargetAngleZ < 0)
	{
		if( this->mAngleZ > this->mTargetAngleZ )
			return false;
		else
			return true;
	}
	else
	{
		if( this->mAngleZ < this->mTargetAngleZ)
			return false;
		else
			return true;
	}
}


bool Map::IsOnPlatform(const float x, const float z) const
{
	Vector3 pos = this->mMesh->GetPosition();

	//calculate the distance from the XZ-center of the Map to the XZ-position
	float distance = sqrt(pow(pos.x - x, 2) + pow(pos.z - z, 2));

	bool isOn = false;
	if(distance < this->mScaledRadius)
		isOn = true;

	return isOn;
}

bool Map::IsInHotZone(Vector3 positionOfBall, float radiusBall)
{
	float distance = (this->mHotZonePosition - positionOfBall).GetLength();
	if(distance <= (this->mHotZoneRadius - radiusBall))
		return true;
	return false;
}

void Map::RotateX(float dt)
{
	float angle = (PI/8.0f)*dt*0.001f;
	if((this->mAngleX+angle) > -this->mMaxAngleX && (this->mAngleX+angle) < this->mMaxAngleX)
	{
		Matrix3 temp;
		temp.SetRotationZ(this->mAngleZ);
		Vector3 xA = Vector3(1,0,0);
		Vector3 xAnew = temp * xA;
		this->mMesh->RotateAxis(xAnew, angle);
		this->mAngleX +=angle;
	}
}

void Map::RotateZ(float dt)
{
	float angle = (PI/8.0f)*dt*0.001f;
	if((this->mAngleZ+angle) > -this->mMaxAngleZ && (this->mAngleZ+angle) < this->mMaxAngleZ)
	{
		/* we don't need the same proc. as for the RotateX 
		   because we don't want to change the rotation axis.
		   if we do change the mesh would rotate around the y-axis from the
		   original base.
		*/
		Vector3 zA = Vector3(0,0,1);
		this->mMesh->RotateAxis(zA, angle);
		this->mAngleZ +=angle;
	}
}

void Map::SetToStartPosition()
{
	this->SetPosition(this->mStartPosition);
}

void Map::Reset()
{
	Vector3 theScale = Vector3(this->mMesh->GetScaling());
	Vector3 inverseScale = theScale.GetInverseComponents();
	this->SetScale(inverseScale);
}

void Map::RotateAxis( Vector3 vec, float angle )
{

}

void Map::SetScale( Vector3 scale )
{
	this->mMesh->Scale(scale);
}

void Map::ResetXZAngles()
{
	this->mAngleX = this->mAngleZ = 0.0f; if(this->mMesh) this->mMesh->ResetRotation();
}

Vector3 Map::GetScale() const
{
	return Vector3(this->mMesh->GetScaling());
}
