#include "ShaderManager.h"
#include <windows.h>
#include <d3d11.h>

vector<ID3D11VertexShader*> ShaderManager::vertexShaders;

void ShaderManager::AddEffect(char* _filePath)
{
	////CONVERTING THE char* to a wchar*
	//size_t origsize = strlen(_filePath) + 1;
	//const size_t newSize = 100;
	//size_t convertedChars = 0;
	//wchar_t wcString[newSize];
	//mbstowcs_s(&convertedChars, wcString, origsize, _filePath, _TRUNCATE);
	//wcscat_s(wcString, L"");
}