#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <windows.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <string>
using std::string;

enum RENDERTARGETS{RENDER_BACKBUFFER, RENDER_GEOMETRYBUFFER, RENDER_LIGHTBUFFER, RENDER_DEPTHBUFFER};

static const int MOUSE_LEFT = 0;
static const int MOUSE_RIGHT = 1;

#define ReleaseCOM(x) if(x != NULL) { x->Release(); x = NULL; }
#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)												\
{															\
	HRESULT hr = (x);										\
	if(FAILED(hr))											\
{															\
	DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true);		\
}															\
}
#endif

#else
#ifndef HR
#define HR(x) (x)
#endif
#endif

namespace Colors
{
	XMGLOBALCONST XMFLOAT4 White     = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMGLOBALCONST XMFLOAT4 Black     = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMGLOBALCONST XMFLOAT4 Red       = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMGLOBALCONST XMFLOAT4 Green     = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMGLOBALCONST XMFLOAT4 Blue      = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMGLOBALCONST XMFLOAT4 Yellow    = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	XMGLOBALCONST XMFLOAT4 Cyan      = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
	XMGLOBALCONST XMFLOAT4 Magenta   = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	
	XMGLOBALCONST XMFLOAT4 Silver    = XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f);
	XMGLOBALCONST XMFLOAT4 LightSteelBlue = XMFLOAT4(0.69f, 0.77f, 0.87f, 1.0f);
}

struct ScreenInformation
{
	static float horizontalRatio;
	static float verticalRatio;
	static float horizontalDifference;
	static float verticalDifference; 
	static int   screenHorizontal;
	static int   screenVertical;

	static bool fullscreen;
	static float aspectRatio;
	static float fov;
	static float orthoRatio;
};

struct TerrainDescription
{
	float width;
	float height;
	int numberOfSegments;
	float smoothingFactor;
	XMFLOAT3 centerPoint;
};

#pragma region INPUT LAYOUT STRUCTURES
struct VertexType
{
	XMFLOAT4 position;
	XMFLOAT2 texture;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 binormal;
};

struct SimpleCubeVertex
{
    XMFLOAT3 pos;   // position
    XMFLOAT3 color; // color
};

struct Pos_Color_Vertex
{
    XMFLOAT3 pos;
    XMFLOAT3 color;
};

struct Pos_Tex_Vertex
{
	XMFLOAT3 pos;
	XMFLOAT2 tex;
};
#pragma endregion


#pragma region CONSTANT STRUCTURES
struct World_ViewProj_ConstantBuffer
{
    XMFLOAT4X4 world;
    XMFLOAT4X4 viewProjection;
};

struct ConstantBuffer
{
    XMFLOAT4X4 world;
    XMFLOAT4X4 viewProjection;
};
#pragma endregion

#endif