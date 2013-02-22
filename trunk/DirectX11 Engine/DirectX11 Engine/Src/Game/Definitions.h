#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <DirectXMath.h>
using namespace DirectX;

#include <string>
using std::string;

#define ReleaseCOM(x) if(x != NULL) { x->Release(); x = NULL; }
#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                              \
{                                                          \
	HRESULT hr = (x);                                      \
	if(FAILED(hr))                                         \
{                                                      \
	DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
}                                                      \
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

struct Vertex
{
	static XMFLOAT3 pos;
	static XMFLOAT4 color;
};

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

#endif