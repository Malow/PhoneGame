#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "Vector.h"

extern "C"
{
	class DECLDIR iBillboard
	{	
		public:
			iBillboard() {};
			virtual ~iBillboard() {};

			/*virtual Vector3 GetPosition() const = 0;
			virtual Vector2 GetTexCoord() const = 0;
			virtual Vector2 GetSize() const = 0;
			virtual Vector3 GetColor() const = 0;
			virtual void SetPosition(Vector3 position) = 0;
			virtual void SetTexCoord(Vector2 texCoord) = 0;
			virtual void SetSize(Vector2 size) = 0;
			virtual void SetColor(Vector3 color) = 0;*/

			virtual void SetRenderShadowFlag(bool flag) = 0;
	};
}