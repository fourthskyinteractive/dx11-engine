#ifndef TERRAINGENERATOR_H_
#define TERRAINGENERATOR_H_

#include "../../Game/Definitions.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <dxgi.h>
using namespace DirectX;
#include <vector>
using namespace std;


class TerrainGenerator
{
public:
	TerrainGenerator();
	TerrainGenerator(const TerrainGenerator&){};
	~TerrainGenerator();

	static void CreateTerrain(float _width, float _height, int _numberOfSegments, float _smoothingFactor, XMFLOAT3 _centerPoint, vector<XMFLOAT3>& _vertices);
	static void SquareStep(int _TL, int _TR, int _BL, int _BR, float _smoothingFactor, vector<XMFLOAT3>& _vertices);
	static void DiamondStep(int _TL, int _TR, int _BL, int _BR, int _centerPoint, float _smoothingFactor, vector<XMFLOAT3>& _vertices);

private:
	static int slicesMade;
};

#endif