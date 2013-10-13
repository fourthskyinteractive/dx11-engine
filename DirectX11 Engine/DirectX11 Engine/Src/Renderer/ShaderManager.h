#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include "../Game/Definitions.h"
#include "Effects/d3dx11effect.h"

#include <atlbase.h>
#include <d3d11.h>
#include <d3d11shader.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <vector>
using namespace std;

enum SHADER_TYPE{VERTEX_SHADER, PIXEL_SHADER, GEOMETRY_SHADER, COMPUTE_SHADER};
enum VERTEX_SHADERS{NO_VERTEX_SHADER = -1, GBUFFER_VERTEX_SHADER, DEFERRED_LIGHTING_VERTEX_SHADER};
enum PIXEL_SHADERS{NO_PIXEL_SHADER = -1, GBUFFER_PIXEL_SHADER, DEFERRED_LIGHTING_PIXEL_SHADER};
enum GEOMETRY_SHADERS{NO_GEOMETRY_SHADER = -1, DEFERRED_LIGHTING_GEOMETRY_SHADER};
enum COMPUTE_SHADERS{NO_COMPUTE_SHADER = -1, DEFERRED_LIGHTING_COMPUTE_SHADER};

struct VertexShader
{
	void* bufferBytes;
	unsigned int byteWidth;
	VERTEX_SHADERS shaderType;
	CComPtr<ID3D11VertexShader> shader;
};

struct GeometryShader
{
	void* bufferBytes;
	unsigned int byteWidth;
	GEOMETRY_SHADERS shaderType;
	CComPtr<ID3D11GeometryShader> shader;
};

struct PixelShader
{
	void* bufferBytes;
	unsigned int byteWidth;
	PIXEL_SHADERS shaderType;
	CComPtr<ID3D11PixelShader> shader;
};

struct ComputeShader
{
	void* bufferBytes;
	unsigned int byteWidth;
	COMPUTE_SHADERS shaderType;
	CComPtr<ID3D11ComputeShader> shader;
};

class ShaderManager
{
public:
	ShaderManager();
	ShaderManager(const ShaderManager&){};
	~ShaderManager();

	static void Initialize();
	static int AddVertexShader(void* _shaderBytes, SIZE_T _byteWidth, VERTEX_SHADERS _shaderType, unsigned int _vertexComponents = 0);
	static int AddPixelShader(void* _shaderBytes, SIZE_T _byteWidth, PIXEL_SHADERS _shaderType);
	static int AddGeometryShader(void* _shaderBytes, SIZE_T _byteWidth, GEOMETRY_SHADERS _shaderType);
	static int AddComputeShader(void* _shaderBytes, SIZE_T _byteWidth, COMPUTE_SHADERS _shaderType);

	static void Shutdown();
	
	static vector<VertexShader> vertexShaders;
	static vector<PixelShader> pixelShaders;
	static vector<GeometryShader> geometryShaders;
	static vector<ComputeShader> computeShaders;
};

#endif