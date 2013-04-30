#include "GeometryMaker.h"


GeometryMaker::GeometryMaker()
{

}

GeometryMaker::GeometryMaker(const GeometryMaker& _geometryMaker)
{

}

GeometryMaker::~GeometryMaker()
{

}

void GeometryMaker::CreateSphere(int _slices, float _radius, XMFLOAT3** _verts, unsigned long** _indices, int* _numVerts, int* _numIndices)
{
	int numVerts = _slices * _slices;
	int numIndices = numVerts * 6;

	XMFLOAT3* vertices = new XMFLOAT3[numVerts];
	unsigned long* indices = new unsigned long[numIndices];
	XMFLOAT3 centerPoint(0.0f, 0.0f, 0.0f);
	XMFLOAT3 rad(_radius, 0.0f, 0.0f);

	//Create the vertices
	for(int i = 0; i < _slices; ++i)
	{
		float difX = 360.0f / _slices;
		for(int j = 0; j < _slices; ++j)
		{
			float difY = 360.0f / _slices;

			XMMATRIX zRot = XMMatrixRotationAxis(XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 1.0f)), j * difY);
			XMMATRIX yRot = XMMatrixRotationAxis(XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f)), i * difX);

			XMFLOAT3 point; 
			XMStoreFloat3(&point, XMVector3Transform(XMVector3Transform(XMLoadFloat3(&rad), zRot), yRot));

			vertices[i + j * _slices] = point;
		}
	}

	//Create the indices
	int indice = 0;
	for(int i = 0; i < _slices; ++i)
	{
		for(int j = 0; j < _slices; ++j)
		{
			int s1 = i == 89 ? 0 : i + 1;
			int s2 = j == 89 ? 0 : j + 1;
			
			short upperLeft = (short)(i * _slices + j);
			short upperRight = (short)(s1 * _slices + j);
			short lowerLeft = (short)(i * _slices + s2);
			short lowerRight = (short)(s1 * _slices + s2);

			indices[indice++] = upperLeft;
			indices[indice++] = upperRight;
			indices[indice++] = lowerLeft;
			indices[indice++] = lowerLeft;
			indices[indice++] = upperRight;
			indices[indice++] = lowerRight;
		}
	}

	*_verts = vertices;
	*_indices = indices;

	*_numVerts = numVerts;
	*_numIndices = numIndices;	
}

void GeometryMaker::CreateCone()
{

}