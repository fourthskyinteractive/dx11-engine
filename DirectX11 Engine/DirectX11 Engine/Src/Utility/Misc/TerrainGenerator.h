#ifndef TERRAINGENERATOR_H_
#define TERRAINGENERATOR_H_

#include "../../Game/Definitions.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <dxgi.h>
using namespace DirectX;
#include <vector>
#include <queue>
using namespace std;

struct Square
{
	int BL;
	int width;
};

struct Diamond
{
	int centerPoint;
	int width;
};

class TerrainGenerator
{
public:

	TerrainGenerator();
	TerrainGenerator(const TerrainGenerator&){};
	~TerrainGenerator();

	static void CreateTerrain(float _width, float _height, int _numberOfSegments, float _smoothingFactor, XMFLOAT3 _centerPoint, vector<XMFLOAT3>& _vertices, vector<unsigned long>& _indices);
	static void SquareStep(int _TL, int _TR, int _BL, int _BR, float _smoothingFactor, vector<XMFLOAT3>& _vertices);
	static void SquareStep(int _centerPoint, int _width, float _smoothingFactor, vector<XMFLOAT3>& _vertices);
	static void DiamondStep(int _TL, int _TR, int _BL, int _BR, int _centerPoint, float _smoothingFactor, vector<XMFLOAT3>& _vertices);
	static void DiamondStep(int _BL, int _width, float _smoothingFactor, vector<XMFLOAT3>& _vertices);

	static void CreateIndexList(vector<unsigned long>& _indices);
	static void CreateNormals(vector<XMFLOAT3>& _vertices);

private:
	static queue<Square> squares;
	static queue<Diamond> diamonds;

	static int slicesMade;
	static float width;
	static float height;
	static int numberOfSegments;
	static float smoothingFactor;
	static XMFLOAT3 centerPoint;
};

#endif