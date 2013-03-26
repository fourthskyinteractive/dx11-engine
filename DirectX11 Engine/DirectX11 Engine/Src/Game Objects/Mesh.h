#ifndef MESH_H
#define MESH_H

#include <d3d11.h>
#include <d3d11shader.h>
#include "../Game/Definitions.h"
#include "../Renderer/TextureManager.h"
#include "../Renderer/Shader Classes/LightShader.h"

class Mesh
{
public:
	struct MeshType
	{
		float x, y, z, w;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};

	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	Mesh();
	Mesh(const Mesh&);
	~Mesh();

	bool Initialize(XMFLOAT3 _pos, XMFLOAT3 _scale, XMFLOAT3 _rotation, char* _meshFilename);
	void Shutdown();
	void Render();

	int GetIndexCount(){return indexCount;}
	int GetVertexCount(){return vertexCount;}
	int GetNumTextures(){return textures->NumberOfTextures();}
	ID3D11ShaderResourceView* GetTexture(int _textureIndex){return textures->GetTexture(_textureIndex);}
	void UpdateWorldMatrix(XMFLOAT3 _pos, XMFLOAT3 _scale, XMFLOAT3 _rotation);
	void AddTexture(WCHAR* _filePath);

private:
	bool InitializeBuffers();
	void ShutdownBuffers();
	void RenderBuffers();

	
	void ReleaseTextures();
	bool LoadMesh(char* _filePath);
	void ReleaseMesh();

	void CalculateMeshVectors();
	void CalculateTangentBinormal(TempVertexType _vertex1, TempVertexType _vertex2, TempVertexType _vertex3,
		VectorType& tangent, VectorType& binormal);
	void CalculateNormal(VectorType _tangent, VectorType _binormal, VectorType& _normal);

private:
	LightShader shaderUsed;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexbuffer;
	int vertexCount;
	int indexCount;
	TextureManager* textures;

	MeshType* mesh;

	XMFLOAT4X4 worldMatrix;
};
#endif