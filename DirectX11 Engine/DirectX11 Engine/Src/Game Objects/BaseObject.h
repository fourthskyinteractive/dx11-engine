#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include <Windows.h>
#include <d3d11.h>
#include <d3d11shader.h>


#include "../Game/Definitions.h"
#include "../Renderer/TextureManager.h"
#include "../Renderer/ShaderManager.h"
#include "../Camera/Camera.h"
//#include "../Game/Game.h"

enum VertexStructure{POS_COLOR};
enum ConstantBufferStructure{WORLD_VIEWPROJ};
enum ObjectType{OBJECT_CUBE, OBJECT_CUBE_N};

struct Pos_Color_Vertex
{
    XMFLOAT3 pos;
    XMFLOAT3 color;
};

struct World_ViewProj_ConstantBuffer
{
    XMFLOAT4X4 world;
    XMFLOAT4X4 viewProjection;
};

class BaseObject
{
public:
	BaseObject();
	BaseObject(const BaseObject&);
	~BaseObject();

	void Initialize(ObjectType _objectType);
	void Remove();
	void AddTexture(WCHAR* _filename);
	void CreateCube(VertexStructure _vertexStructure);
	void SetWorldMatrix(XMFLOAT3 _pos, float _scale, XMFLOAT3 _rotationDegrees);
	void SetConstantBuffer();
	void SetRendererParameters();

	//Stores the transpose of the world matrix so that it is ready to go to the shader
	VERTEX_SHADERS vertexShader;
	PIXEL_SHADERS pixelShader;

	XMMATRIX worldMatrix;
	TextureManager* textureManager;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int vertexSize;
	int indexCount;
	ID3D11Buffer* constantBuffer;
	ID3D11InputLayout* inputLayout;
	ConstantBuffer constantBufferData;
};

#endif