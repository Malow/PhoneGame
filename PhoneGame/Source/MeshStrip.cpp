#include "MeshStrip.h"
#include "Graphics.h"
MeshStrip::MeshStrip()
{
	this->indicies = NULL;
	this->mesh = NULL;
	this->nrOfIndicies = 0;
	this->nrOfVerts = 0;
}
	
MeshStrip::~MeshStrip() 
{
	if(this->mesh)
		delete [] this->mesh;
	if(this->indicies)
		delete [] this->indicies;
}

MeshStrip::MeshStrip(const MeshStrip* origObj)
{
	this->mesh = new Vertex(origObj->mesh);
	this->nrOfVerts = origObj->nrOfVerts;
	this->nrOfIndicies = origObj->nrOfIndicies;
	this->indicies = new int[origObj->nrOfIndicies];
	for(int i = 0; i < origObj->nrOfIndicies; i++)
	{
		this->indicies[i] = origObj->indicies[i];
	}
}