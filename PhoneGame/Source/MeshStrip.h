#ifndef MESHSTRIP
#define MESHSTRIP

#include "Vertex.h"

class MeshStrip
{
private:
	Vertex* mesh;
	int nrOfVerts;
	int nrOfIndicies;
	int* indicies;

public:
	MeshStrip();
	virtual ~MeshStrip();
	MeshStrip(const MeshStrip* origObj);

	Vertex* getVerts() const { return this->mesh; }
	void SetVerts(Vertex* verts) { this->mesh = verts; }
	int getNrOfVerts() const { return this->nrOfVerts; }
	void setNrOfVerts(int vertno) { this->nrOfVerts = vertno; }
	int* getIndicies() const { return this->indicies; }
	void SetIndicies(int* inds) { this->indicies = inds; }
	int getNrOfIndicies() const { return this->nrOfIndicies; }
	void setNrOfIndicies(int indcount) { this->nrOfIndicies = indcount; }
};

#endif