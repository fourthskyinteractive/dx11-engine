#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator()
{

}

TerrainGenerator::~TerrainGenerator()
{

}

void TerrainGenerator::CreateTerrain(float _width, float _height, int _numberOfSegments, int _smoothingFactor, XMFLOAT3 _centerPoint, vector<XMFLOAT3>& _vertices)
{
	//NUMBER OF SEGMENTS IS PER SIDE
	
	//SO FOR 2 SEGMENTS YOU WOULD GET THIS
	//-----------
	//|	   |	|
	//|	   |	|
	//===========
	//|	   |	|
	//|	   |	|		OR WHATEVER
	//-----------

	int numVerts = (_numberOfSegments + 1) * (_numberOfSegments + 1);
	_vertices.resize(numVerts);

	for(int i = 0; i < _numberOfSegments + 1; ++i)
	{
		for(int j = 0; j < _numberOfSegments + 1; ++j)
		{
			int currentVert = i * (_numberOfSegments + 1) + j;
			_vertices[currentVert] = XMFLOAT3((_centerPoint.x - (_width / 2.0f)) + j * (_width / _numberOfSegments), _centerPoint.y, (_centerPoint.z - (_height / 2.0f)) + i * (_height / _numberOfSegments));
		}
	}

	int TL = _numberOfSegments * (_numberOfSegments + 1);
	int TR = TL + _numberOfSegments;
	int BL = 0;
	int BR = _numberOfSegments;

	_vertices[TL].y = 20;
	_vertices[TR].y = 20;
	_vertices[BL].y = 20;
	_vertices[BR].y = 20;

	SquareStep(TL, TR, BL, BR, _vertices);
}

void TerrainGenerator::SquareStep(int _TL, int _TR, int _BL, int _BR, vector<XMFLOAT3>& _vertices)
{
	int centerPoint = ((_BR - _BL) / 2) + _BL;
	centerPoint += (_BR - _BL + 1) * ((_BR - _BL) / 2);
}