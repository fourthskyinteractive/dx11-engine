#include "TerrainGenerator.h"
#include "../../Game/Game.h"



int				TerrainGenerator::slicesMade;
float			TerrainGenerator::width;
float			TerrainGenerator::height;
int				TerrainGenerator::numberOfSegments;
float			TerrainGenerator::smoothingFactor;
queue<Square>	TerrainGenerator::squares;
queue<Diamond>	TerrainGenerator::diamonds;

TerrainGenerator::TerrainGenerator()
{

}

TerrainGenerator::~TerrainGenerator()
{

}

void TerrainGenerator::CreateTerrain(float _width, float _height, int _numberOfSegments, float _smoothingFactor, XMFLOAT3 _centerPoint, vector<XMFLOAT3>& _vertices)
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
	width = _width;
	height = _height;
	numberOfSegments = _numberOfSegments;
	smoothingFactor = _smoothingFactor;


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

	_vertices[TL].y = 0;
	_vertices[TR].y = 0;
	_vertices[BL].y = 0;
	_vertices[BR].y = 0;

	Square firstSquare;
	firstSquare.BL = BL;
	firstSquare.width = BR - BL;
	if(firstSquare.width >= 1)
		squares.push(firstSquare);

	int slicesToMake = numberOfSegments;
	while(true)
	{
		if(squares.empty() && diamonds.empty())
			break;
		//DO THE DIAMOND PASS BY FINDING THE CENTER AND GIVING IT A VALUE
		while(!squares.empty())
			DiamondStep(squares.front().BL, slicesToMake, smoothingFactor, _vertices);

		while(!diamonds.empty())
			SquareStep(diamonds.front().centerPoint, slicesToMake, smoothingFactor, _vertices);

		slicesToMake = slicesToMake / 2;
		smoothingFactor -= _smoothingFactor / numberOfSegments;
	}
}

void TerrainGenerator::DiamondStep(int _BL, int _width, float _smoothingFactor, vector<XMFLOAT3>& _vertices)
{
	if(_width < 2)
	{
		squares.pop();
		return;
	}

	int BL = _BL;
	int BR = BL + _width;
	int TL = BL + _width * numberOfSegments + _width;
	int TR = BR + _width * numberOfSegments + _width;

	int centerPoint = (_width / 2) + BL;
	centerPoint += ((numberOfSegments + 1) * (_width / 2));
	
	float averageOfCorners = (_vertices[BR].y + _vertices[BL].y + _vertices[TR].y + _vertices[TL].y) / 4.0f;
	float scaling = 0.0f;
	scaling = (float)rand()/((float)RAND_MAX/(_smoothingFactor * 2)) - _smoothingFactor;
	_vertices[centerPoint].y = averageOfCorners + scaling;

	Diamond tempDiamond;	

	tempDiamond.centerPoint = centerPoint;

	tempDiamond.width = _width;

	diamonds.push(tempDiamond);

	squares.pop();
}

void TerrainGenerator::SquareStep(int _centerPoint, int _width, float _smoothingFactor, vector<XMFLOAT3>& _vertices)
{
	if(_width < 2)
	{
		diamonds.pop();
		return;
	}

	int TM = _centerPoint + ((numberOfSegments + 1) * (_width / 2));;
	int LM = _centerPoint - (_width / 2);
	int RM = _centerPoint + (_width / 2);;
	int BM = _centerPoint - ((numberOfSegments + 1) * (_width / 2));;

	int BL = BM - (_width / 2);
	int BR = BM + (_width / 2);
	int TL = TM - (_width / 2);
	int TR = TM + (_width / 2);

	float averageOfDiamond = 0.0f;
	float scaling = 0.0f;
	//TM
	averageOfDiamond = (_vertices[TL].y + _vertices[TR].y + _vertices[_centerPoint].y) / 3.0f;
	scaling = (float)rand()/((float)RAND_MAX/(_smoothingFactor * 2)) - _smoothingFactor;
	_vertices[TM].y = averageOfDiamond + scaling;
	//RM
	averageOfDiamond = (_vertices[TR].y + _vertices[BR].y + _vertices[_centerPoint].y) / 3.0f;
	scaling = (float)rand()/((float)RAND_MAX/(_smoothingFactor * 2)) - _smoothingFactor;
	_vertices[RM].y = averageOfDiamond + scaling;
	//LM
	averageOfDiamond = (_vertices[TL].y + _vertices[BL].y + _vertices[_centerPoint].y) / 3.0f;
	scaling = (float)rand()/((float)RAND_MAX/(_smoothingFactor * 2)) - _smoothingFactor;
	_vertices[LM].y = averageOfDiamond + scaling;
	//BM
	averageOfDiamond = (_vertices[BL].y + _vertices[BR].y + _vertices[_centerPoint].y) / 3.0f;
	scaling = (float)rand()/((float)RAND_MAX/(_smoothingFactor * 2)) - _smoothingFactor;
	_vertices[BM].y = averageOfDiamond + scaling;

	Square tempSquare;
	//Square 1
	tempSquare.BL = LM;
	tempSquare.width = _width;
	squares.push(tempSquare);
	//Square 2
	tempSquare.BL = _centerPoint;
	squares.push(tempSquare);
	//Square 3
	tempSquare.BL = BL;
	squares.push(tempSquare);
	//Square 4
	tempSquare.BL = BM;
	squares.push(tempSquare);

	diamonds.pop();
}