#pragma once
/**
* Author: Jerry Rahmqvist
* Create Date: 09/05/2012
* 
* This Class is the abstract class for game objects.
*
**/

#include "Vector.h"

class GameObject
{
	protected:
			class iMesh* mMesh;
			Vector3 mStartPosition;
	public:
			GameObject();
			virtual ~GameObject();
		
			/*! Returns the position of the game object in world space (retrieved from mMesh). */
			Vector3 GetPosition() const;

			/*! Returns the position of the game object in world space (retrieved from mMesh). */
			Vector3 GetPositionVector3() const;

			/*! Returns the mesh of the game object. */
			class iMesh* GetMesh() const;
		
			/*! Sets the position of the game object in world space. */
			void SetPosition(const Vector3 position);
			
			/*! Sets the position of the game obejct in world space. */
			void SetPosition(const float x, const float y, const float z);

			/*! Sets the game object to its startposition. */
			virtual void SetToStartPosition() = 0;

			/*! Sets the mesh of the game object. */
			void SetMesh(class iMesh* mesh);

};