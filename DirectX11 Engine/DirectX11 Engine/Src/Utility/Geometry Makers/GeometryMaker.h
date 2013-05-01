#ifndef GEOMETRYMAKER_H
#define GEOMETRYMAKER_H

#include <d3d11.h>
#include <d3d11shader.h>
#include <vector>
using namespace std;

#include "../../Game/Definitions.h"

class GeometryMaker
{
public:
	GeometryMaker();
	GeometryMaker(const GeometryMaker& _geometryMaker);
	~GeometryMaker();

	static unsigned long FindMiddlePoint(vector<XMFLOAT3>* _verts, vector<unsigned long>* _indices, vector<unsigned long>* _middleIndices, unsigned long _point1, unsigned long _point2);
	static void CreateSphere(int _slices, float _radius, XMFLOAT3** _verts, unsigned long** _indices, int* _numVerts, int* numIndices);
	static void CreateCone();
};
#endif