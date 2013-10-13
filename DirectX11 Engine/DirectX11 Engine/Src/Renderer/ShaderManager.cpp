#include "ShaderManager.h"
#include "D3D11Renderer.h"
#include <fstream>
#include <d3d11.h>

vector<VertexShader>	ShaderManager::vertexShaders;
vector<PixelShader>		ShaderManager::pixelShaders;
vector<GeometryShader>	ShaderManager::geometryShaders;
vector<ComputeShader>	ShaderManager::computeShaders;

void ShaderManager::Initialize()
{

}

int ShaderManager::AddVertexShader(void* _shaderBytes, SIZE_T _byteWidth, VERTEX_SHADERS _shaderType, unsigned int _vertexComponents)
{
	HRESULT hr;
	VertexShader tempVertexShader;

	tempVertexShader.byteWidth = _byteWidth;
	tempVertexShader.bufferBytes = _shaderBytes;

	CComPtr<ID3D11VertexShader> vShader;
	hr = D3D11Renderer::d3dDevice->CreateVertexShader(_shaderBytes, _byteWidth, NULL, &vShader);

	if(hr == S_OK)
	{
		tempVertexShader.shaderType = _shaderType;
		tempVertexShader.shader = vShader;
		vertexShaders.push_back(tempVertexShader);
		return vertexShaders.size() - 1;
	}

	return -1;
}

int ShaderManager::AddPixelShader(void* _shaderBytes, SIZE_T _byteWidth, PIXEL_SHADERS _shaderType)
{
	HRESULT hr;
	PixelShader tempPixelShader;

	tempPixelShader.byteWidth = _byteWidth;
	tempPixelShader.bufferBytes = _shaderBytes;

	CComPtr<ID3D11PixelShader> pShader;
	hr = D3D11Renderer::d3dDevice->CreatePixelShader(_shaderBytes, _byteWidth, NULL, &pShader);

	if(hr == S_OK)
	{
		tempPixelShader.shaderType = _shaderType;
		tempPixelShader.shader = pShader;
		pixelShaders.push_back(tempPixelShader);
		return pixelShaders.size() - 1;
	}

	return -1;
}

int ShaderManager::AddGeometryShader(void* _shaderBytes, SIZE_T _byteWidth, GEOMETRY_SHADERS _shaderType)
{
	HRESULT hr;
	GeometryShader tempGeometryShader;

	tempGeometryShader.byteWidth = _byteWidth;
	tempGeometryShader.bufferBytes = _shaderBytes;

	CComPtr<ID3D11GeometryShader> gShader;
	hr = D3D11Renderer::d3dDevice->CreateGeometryShader(_shaderBytes, _byteWidth, NULL, &gShader);

	if(hr == S_OK)
	{
		tempGeometryShader.shaderType = _shaderType;
		tempGeometryShader.shader = gShader;
		geometryShaders.push_back(tempGeometryShader);
		return (geometryShaders.size() - 1);
	}

	return -1;
}

int ShaderManager::AddComputeShader(void* _shaderBytes, SIZE_T _byteWidth, COMPUTE_SHADERS _shaderType)
{
	HRESULT hr;
	ComputeShader tempComputeShader;

	tempComputeShader.byteWidth = _byteWidth;
	tempComputeShader.bufferBytes = _shaderBytes;

	CComPtr<ID3D11ComputeShader> cShader;
	hr = D3D11Renderer::d3dDevice->CreateComputeShader(_shaderBytes, _byteWidth, NULL, &cShader);

	if(hr == S_OK)
	{
		tempComputeShader.shaderType = _shaderType;
		tempComputeShader.shader = cShader;
		computeShaders.push_back(tempComputeShader);
		return (geometryShaders.size() - 1);
	}

	return -1;
}

void ShaderManager::Shutdown()
{
	
}