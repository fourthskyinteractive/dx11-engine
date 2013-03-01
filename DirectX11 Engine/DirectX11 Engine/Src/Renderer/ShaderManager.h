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

enum VERTEX_SHADERS{BASIC_SHADER = 0};

class ShaderManager
{
public:
	ShaderManager();
	ShaderManager(const ShaderManager&){};
	~ShaderManager();

	static void Initialize();
	static int AddShader(char* _filePath);
	
	static vector<ID3D11VertexShader*> vertexShaders;
};

#endif