#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include "../Game/Definitions.h"
#include "Effects/d3dx11effect.h"
#include <d3d11.h>
#include <d3d11shader.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <vector>
using namespace std;

enum SHADER_TYPE{VERTEX_SHADER, PIXEL_SHADER};
enum VERTEX_SHADERS{COLOR_VERTEX_SHADER = 0, TEXTURE_VERTEX_SHADER, MULTIPLE_TEXTURE_VERTEX_SHADER, DEPTH_VERTEX_SHADER};
enum PIXEL_SHADERS{COLOR_PIXEL_SHADER = 0, TEXTURE_PIXEL_SHADER, MULTIPLE_TEXTURE_PIXEL_SHADER, DEPTH_PIXEL_SHADER};

struct VertexShader
{
	ID3DBlob* buffer;
	ID3D11VertexShader* shader;
};

struct PixelShader
{
	ID3DBlob* buffer;
	ID3D11PixelShader* shader;
};

class ShaderManager
{
public:
	ShaderManager();
	ShaderManager(const ShaderManager&){};
	~ShaderManager();

	static void Initialize();
	static int AddShader(char* _filePath, SHADER_TYPE _shaderType);
	static void Shutdown();
	
	static vector<VertexShader> vertexShaders;
	static vector<PixelShader> pixelShaders;
};

#endif