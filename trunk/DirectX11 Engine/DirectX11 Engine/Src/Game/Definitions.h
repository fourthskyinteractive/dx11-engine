#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <windows.h>
#include <atlbase.h>

#include <DirectXMath.h>
using namespace DirectX;

#include <string>
using std::string;

#include <atlbase.h>
#include <d3d11.h>
#include <dxgi.h>
using namespace DirectX;

#include <d3d11.h>
#include <d3d11shader.h>
#include <vector>
using namespace std;


//enum RENDERCOMPONENTS{VERTEX_BUFFER_COMPONENT, INDEX_BUFFER_COMPONENT, CONSTANT_BUFFER_COMPONENT, TEXTURE_COMPONENT, ANIMATION_COMPONENT};
//enum VERTEXBUFFERCOMPONENTS{POSITION_VERTEX_COMPONENT, NORMAL_VERTEX_COMPONENT, UV_VERTEX_COMPONENT};
//
//enum CONSTANTBUFFERCOMPONENT{WORLDMATRIX_CONSTANT_COMPONENT, VIEWMATRIX_CONSTANT_COMPONENT, PROJECTIONMATRIX_CONSTANT_COMPONENT};
//enum TEXTURECOMPONENTS{DIFFUSE_TEXTURE_COMPONENT, NORMAL_TEXTURE_COMPONENT, SPECULAR_TEXTURE_COMPONENT};


enum RENDERTARGETS{RENDER_BACKBUFFER, RENDER_GEOMETRYBUFFER, RENDER_LIGHTBUFFER, RENDER_DEPTHBUFFER};
enum BUFFER_TYPE{VERTEX_BUFFER, INDEX_BUFFER};

enum COMPUTEBUFFERTYPE{COMPUTE_NO_TYPE_BUFFER = -1, COMPUTE_BACK_BUFFER, COMPUTE_MISC_BUFFER};

static const int MOUSE_LEFT = 0;
static const int MOUSE_RIGHT = 1;

#define ScreenWidth 1024
#define ScreenHeight 768

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

#define WAIT_FOR_THREAD(_thread) if((_thread).joinable()) (_thread).join();

#define SAFE_DELETE(memoryToDelete) if(memoryToDelete){ delete memoryToDelete; memoryToDelete = NULL;}

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

static wchar_t* ConvertConstCharToWChar(const char* _charToConvert)
{
	// newsize describes the length of the 
	// wchar_t string called wcstring in terms of the number 
	// of wide characters, not the number of bytes.
	size_t newsize = strlen(_charToConvert) + 1;

	// The following creates a buffer large enough to contain 
	// the exact number of characters in the original string
	// in the new format. If you want to add more characters
	// to the end of the string, increase the value of newsize
	// to increase the size of the buffer.
	wchar_t * wcstring = new wchar_t[newsize];

	// Convert char* string to a wchar_t* string.
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, newsize, _charToConvert, _TRUNCATE);
	// Display the result and indicate the type of string that it is.

	return wcstring;
}	

struct BuffersForBinding
{
	BUFFER_TYPE bufferType;

	unsigned int numBuffers;

	vector<CComPtr<ID3D11Buffer>> buffers;
	vector<unsigned int> strides;
	vector<unsigned int> offsets;
};

struct ComputeShaderBuffers
{
	vector<COMPUTEBUFFERTYPE> computeBufferType;
	vector<CComPtr<ID3D11Buffer>> computeDataBuffers;
	vector<CComPtr<ID3D11UnorderedAccessView>> computeUAVs;
	vector<CComPtr<ID3D11ShaderResourceView>> computeSRVs;
};

struct DX11RenderDataMembers
{
	void* vertexBufferBytes;
	unsigned int vertexBufferBiteWidth;
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11GeometryShader> geometryShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11ComputeShader> computeShader;

	CComPtr<ID3D11SamplerState> samplerState;
	CComPtr<ID3D11InputLayout> inputLayout;

	ComputeShaderBuffers computeShaderBuffers;

	vector<BuffersForBinding> buffersToBind;
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

struct ModelData
{
	vector<XMFLOAT4> positions;
	vector<XMFLOAT2> texCoords;
	vector<XMFLOAT3> normals;
	vector<XMFLOAT3> tangents;
	vector<XMFLOAT3> binormals;

	vector<unsigned long> indices;
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