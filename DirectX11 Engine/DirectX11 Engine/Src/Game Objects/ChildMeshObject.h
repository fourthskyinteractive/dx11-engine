#ifndef CHILDMESHOBJECT_H
#define CHILDMESHOBJECT_H

#include <Windows.h>
#include <d3d11.h>
#include <d3d11shader.h>

#include "../Game/Definitions.h"
#include "../Renderer/TextureManager.h"
#include "../Renderer/Shader Classes/LightShader.h"

class ParentMeshObject;

class ChildMeshObject
{
private:
	struct VertexType
	{
		XMFLOAT4 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 localMatrix;
	XMFLOAT3 position;
	XMFLOAT3 scale;
	XMFLOAT3 rotation;

	LightShader shaderUsed;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexbuffer;
	int vertexCount;
	int indexCount;
	TextureManager* textures;

	ParentMeshObject* parent;

public:
	ChildMeshObject();
	ChildMeshObject(const ChildMeshObject& _childMeshObject);
	~ChildMeshObject();

	XMFLOAT4X4 GetLocalMatrixF(){return localMatrix;}
	XMMATRIX GetLocalMatrixM(){return XMLoadFloat4x4(&localMatrix);}
	XMFLOAT4X4 GetWorldMatrixF(){return worldMatrix;}
	XMMATRIX GetWorldMatrixM(){return XMLoadFloat4x4(&worldMatrix);}
	XMFLOAT3 GetPosition(){return position;}
	XMFLOAT3 GetRotation(){return rotation;}
	XMFLOAT3 GetScale(){return scale;}

	ParentMeshObject* GetParent(){return parent;}

	void SetLocalMatrixF(XMFLOAT4X4 _localMatrix){localMatrix = _localMatrix;}
	void SetWorldMatrixF(XMFLOAT4X4 _worldMatrix){localMatrix = _worldMatrix;}
	void SetPosition(XMFLOAT3 _position){;}
	void SetRotation(XMFLOAT3 _rotation){rotation = _rotation;}
	void SetScale(XMFLOAT3 _scale){scale = _scale;}

	void SetParent(ParentMeshObject* _parent){parent = _parent;}

	void Initialize(XMFLOAT3 _position, XMFLOAT3 _scale, XMFLOAT3 _rotation);
	void Render();	
	void UpdateLocalMatrix();
	void UpdateWorldMatrix();
};
#endif
