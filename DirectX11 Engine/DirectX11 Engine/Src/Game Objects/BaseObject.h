#ifndef BASEOBJECT_H
#define BASEOBJECT_H

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

struct ShaderVariables
{
	VERTEX_SHADERS vertexShader;
	PIXEL_SHADERS pixelShader;
	TextureManager* textureManager;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int vertexSize;
	int indexCount;
	ID3D11Buffer* constantBuffer;
	ID3D11InputLayout* inputLayout;
	ConstantBuffer constantBufferData;
};

struct ObjectVariables
{
	XMFLOAT3 position;
	XMFLOAT3 scale;
	XMFLOAT3 rotation;

	XMMATRIX worldMatrix;
};

class BaseObject
{
public:
	ObjectVariables objectVariables;
	ShaderVariables shaderVariables;

	BaseObject();
	BaseObject(const BaseObject&);
	~BaseObject();

	void Initialize(ObjectType _objectType);
	void Render();
	void Remove();
	void AddTexture(WCHAR* _filename);
	void CreateCube(VertexStructure _vertexStructure);
	void SetPosition(XMFLOAT3 _pos){objectVariables.position = _pos;}
	void SetScale(XMFLOAT3 _scale){objectVariables.scale = _scale;}
	void SetRotation(XMFLOAT3 _rot){objectVariables.rotation = _rot;}
	void SetWorldMatrix();
	void SetConstantBuffer();
	void SetRendererParameters();
	void UpdateConstantBuffer();	
};

#endif