#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <Windows.h>
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
enum VERTEX_SHADERS{BASIC_VERTEX_SHADER = 0};
enum PIXEL_SHADERS{BASIC_PIXEL_SHADER = 0};

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
	
	static vector<VertexShader> vertexShaders;
	static vector<PixelShader> pixelShaders;
};

#endif