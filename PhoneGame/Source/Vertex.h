#ifndef VERTEX_H
#define VERTEX_H

#include "Vector.h"


struct Vertex
{
	Vector3 pos;
	Vector2 texCoord;
	Vector3 normal;
	Vector3 color;

	Vertex(Vector3 _pos, Vector2 _texCoord, Vector3 _norm, Vector3 _col)
	{
		pos = _pos;
		texCoord = _texCoord;
		normal = _norm;
		color = _col;
	}
	Vertex(Vector3 _pos, Vector2 _texCoord, Vector3 _norm)
	{
		pos = _pos;
		texCoord = _texCoord;
		normal = _norm;
		color = Vector3(0, 0, 0);
	}
	Vertex(Vector3 _pos, Vector3 _norm, Vector3 _color)
	{
		pos = _pos;
		texCoord = Vector2(0, 0);
		normal = _norm;
		color = _color;
	}
	Vertex()
	{
		pos = Vector3(0, 0, 0);
		texCoord = Vector2(0, 0);
		normal = Vector3(0, 0, 0);
		color = Vector3(0, 0, 0);
	}
	Vertex(const Vertex* origObj)
	{
		pos = origObj->pos;
		texCoord = origObj->texCoord;
		normal = origObj->normal;
		color = origObj->color;
	}
};

struct ParticleVertex
{
	Vector3 pos;
	Vector2 dimensions;
	float opacity;
	
	ParticleVertex(Vector3 p, Vector2 d, float op)
	{
		pos = p;
		dimensions = d;
		opacity = op;
	}

	ParticleVertex()
	{
		pos = Vector3(0, 0, 0);
		dimensions = Vector2(0, 0);
		opacity = 0.0f;
	}
};

#endif