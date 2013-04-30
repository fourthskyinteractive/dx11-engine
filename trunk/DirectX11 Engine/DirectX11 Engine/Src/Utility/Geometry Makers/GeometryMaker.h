#ifndef GEOMETRYMAKER_H
#define GEOMETRYMAKER_H

#include <d3d11.h>
#include <d3d11shader.h>
#include "../../Game/Definitions.h"

class GeometryMaker
{
public:
	GeometryMaker();
	GeometryMaker(const GeometryMaker& _geometryMaker);
	~GeometryMaker();

	static void CreateSphere(int _slices, float _radius, XMFLOAT3** _verts, unsigned long** _indices, int* _numVerts, int* numIndices);
	static void CreateCone();
};
#endif