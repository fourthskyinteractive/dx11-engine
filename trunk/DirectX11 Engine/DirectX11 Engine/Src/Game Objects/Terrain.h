#ifndef TERRAIN_H
#define TERRAIN_H

#include <d3d11.h>
#include <d3d11shader.h>

#include "../Game/Definitions.h"
#include "../Renderer/TextureManager.h"
#include "../Renderer/Shader Classes/TerrainShader.h"

class ParentMeshObject;
class DepthShader;

class Terrain
{
private:
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexbuffer;

	ID3D11ShaderResourceView* texture;
	ID3D11RenderTargetView* renderTarget;

	TerrainShader shaderUsed;

	vector<XMFLOAT3> verts;
	vector<unsigned long> indices;

	int vertexCount;
	int indexCount;

	float smallest;
	float largest;

public:
	Terrain();
	Terrain(const Terrain& _terrain);
	~Terrain();

	void Initialize(ID3D11RenderTargetView* _renderTargetView, ID3D11ShaderResourceView* _shaderResourceView, TerrainDescription _terrainDescription);
	void InitializeBuffers();
	void SetShaderBuffers();
	void ChangeShaderResourceView(ID3D11ShaderResourceView* _shaderResourceView);
	void Render();
	void Update();

	//D3DX Buffer Initialization
	static bool InitializeBuffers(VertexType* _vertices, unsigned long* _indices);
};
#endif
