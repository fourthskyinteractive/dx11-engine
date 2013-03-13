#ifndef CUBEOBJECTTEXTURE_H
#define CUBEOBJECTTEXTURE_H

#include <Windows.h>
#include <d3d11.h>
#include <d3d11shader.h>
#include "../Game/Definitions.h"
#include "../Renderer/TextureManager.h"
#include "../Renderer/Shader Classes/LightShader.h"

class CubeObjectTexture
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	CubeObjectTexture();
	CubeObjectTexture(const CubeObjectTexture&);
	~CubeObjectTexture();

	bool Initialize(XMFLOAT3 _pos, XMFLOAT3 _scale, XMFLOAT3 _rotation, char* _modelFilename, WCHAR* _textureFilename);
	void Shutdown();
	void Render();

	int GetIndexCount(){return indexCount;}
	int GetVertexCount(){return vertexCount;}
	int GetNumTextures(){return textures->NumberOfTextures();}
	ID3D11ShaderResourceView* GetTexture(int _textureIndex){return textures->GetTexture(_textureIndex);}
	void UpdateWorldMatrix(XMFLOAT3 _pos, XMFLOAT3 _scale, XMFLOAT3 _rotation);

private:
	bool InitializeBuffers();
	void ShutdownBuffers();
	void RenderBuffers();

	bool AddTexture(WCHAR* _filePath);
	void ReleaseTextures();
	bool LoadModel(char* _filePath);
	void ReleaseModel();

private:
	LightShader shaderUsed;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexbuffer;
	int vertexCount;
	int indexCount;
	TextureManager* textures;

	ModelType* model;

	XMFLOAT4X4 worldMatrix;
};
#endif