#include "ShaderManager.h"
#include "D3D11Renderer.h"
#include <fstream>
#include <windows.h>
#include <d3d11.h>

vector<VertexShader>	ShaderManager::vertexShaders;
vector<PixelShader>		ShaderManager::pixelShaders;

void ShaderManager::Initialize()
{

}

int ShaderManager::AddShader(char* _filePath, SHADER_TYPE _shaderType)
{
	////CONVERTING THE char* to a wchar*
	size_t origsize = strlen(_filePath) + 1;
	const size_t newSize = 100;
	size_t convertedChars = 0;
	wchar_t wcString[newSize];
	mbstowcs_s(&convertedChars, wcString, origsize, _filePath, _TRUNCATE);
	wcscat_s(wcString, L"");

	ID3DBlob* shaderBuffer;

	D3DReadFileToBlob(wcString, &shaderBuffer);
	HRESULT hr;

	if(_shaderType == VERTEX_SHADER)
	{
		ID3D11VertexShader* VS;
		hr = D3D11Renderer::d3dDevice->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &VS);

		if(hr != S_OK)
		{
			string errorStr = "Failed to load Compiled Shader ";
			errorStr += _filePath;

			MessageBox(0, errorStr.c_str(), 0, 0);
			return -1;
		}

		VertexShader vertexShader;
		vertexShader.buffer = shaderBuffer;
		vertexShader.shader = VS;

		vertexShaders.push_back(vertexShader);

		return vertexShaders.size() - 1;
	}

	else if(_shaderType == PIXEL_SHADER)
	{
		ID3D11PixelShader* PS;
		hr = D3D11Renderer::d3dDevice->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &PS);

		if(hr != S_OK)
		{
			string errorStr = "Failed to load Compiled Shader ";
			errorStr += _filePath;

			MessageBox(0, errorStr.c_str(), 0, 0);
			return -1;
		}
		PixelShader pixelShader;
		pixelShader.buffer = shaderBuffer;
		pixelShader.shader = PS;

		pixelShaders.push_back(pixelShader);

		return pixelShaders.size() - 1;
	}

	return -1;
}

void ShaderManager::Shutdown()
{
	for(unsigned int i = 0; i < vertexShaders.size(); ++i)
	{
		if(vertexShaders[i].shader)
		{
			vertexShaders[i].shader->Release();
			vertexShaders[i].shader = 0;
		}
	}

	for(unsigned int i = 0; i < pixelShaders.size(); ++i)
	{
		if(vertexShaders[i].shader)
		{
			pixelShaders[i].shader->Release();
			pixelShaders[i].shader = 0;
		}
	}
}