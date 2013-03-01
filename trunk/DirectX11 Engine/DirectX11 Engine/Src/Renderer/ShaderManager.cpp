#include "ShaderManager.h"
#include "D3D11Renderer.h"
#include <fstream>
#include <windows.h>
#include <d3d11.h>

vector<ID3D11VertexShader*> ShaderManager::vertexShaders;

int ShaderManager::AddShader(char* _filePath)
{
	////CONVERTING THE char* to a wchar*
	size_t origsize = strlen(_filePath) + 1;
	const size_t newSize = 100;
	size_t convertedChars = 0;
	wchar_t wcString[newSize];
	mbstowcs_s(&convertedChars, wcString, origsize, _filePath, _TRUNCATE);
	wcscat_s(wcString, L"");

	ID3D11VertexShader* VS;
	ID3DBlob* VSBuffer;

	D3DReadFileToBlob(wcString, &VSBuffer);

	int siz = VSBuffer->GetBufferSize();
	HRESULT hr = D3D11Renderer::d3dDevice->CreateVertexShader(VSBuffer->GetBufferPointer(), VSBuffer->GetBufferSize(), NULL, &VS);

	vertexShaders.push_back(VS);

	return vertexShaders.size() - 1;
}