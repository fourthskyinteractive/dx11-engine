#ifndef PARENTMESHOBJECT_H
#define PARENTMESHOBJECT_H


#include "../Game/Definitions.h"
#include "../Renderer/Shader Classes/ObjectShader.h"
#include "../Renderer/Shader Classes/DepthShader.h"
#include "../Renderer/Shader Classes/DeferredShader.h"
#include "../Renderer/Shader Classes/LightShader.h"
#include "../Renderer/TextureManager.h"


#include <d3d11.h>
#include <d3d11shader.h>
#include <vector>
using namespace std;



class ChildMeshObject;

class ParentMeshObject
{
private:
	XMFLOAT4X4	worldMatrix;
	XMFLOAT3	position;
	XMFLOAT3	rotation;
	XMFLOAT3	scale;

	BaseShader* shaderUsed;
	DepthShader depthShader;
	ObjectShader objectShader;
	//LightShader lightShader;

	vector<ChildMeshObject*> children;

public:
	TextureManager* textures;

	ParentMeshObject();
	ParentMeshObject(const ParentMeshObject& _parentMeshObject);
	~ParentMeshObject();


	XMFLOAT4X4 GetWorldMatrixF(){return worldMatrix;}
	XMMATRIX GetWorldMatrixM(){return XMLoadFloat4x4(&worldMatrix);}
	XMFLOAT3 GetPosition(){return position;}
	XMFLOAT3 GetRotation(){return rotation;}
	XMFLOAT3 GetScale(){return scale;}

	
	void SetWorldMatrixF(XMFLOAT4X4 _worldMatrix){worldMatrix = _worldMatrix;}
	void SetPosition(XMFLOAT3 _position){position = _position;}
	void SetRotation(XMFLOAT3 _rotation){rotation = _rotation;}
	void SetScale(XMFLOAT3 _scale){scale = _scale;}
	void Initialize(char* _filePath, XMFLOAT3 _position, XMFLOAT3 _scale, XMFLOAT3 _rotation, SHADER_TO_USE _shaderToUse, bool _hasOneTexture /* = false */, WCHAR* _textureFilePath /* = L"" */);
	void AddChild(VertexType* _vertices, unsigned long* _indices, int _numVertices, int _numIdices);
	void Render();
	void AddTexture(WCHAR* _filePath);
	void AddChildTexture(WCHAR* _filePath, int _child);
	void SwitchRenderMode(int _renderMode);
	void SetShaderUsed(BaseShader* _shaderUsed);

	void UpdateWorldMatrix();
	void Update(float _dt);
	void Rotate(float _degrees, XMFLOAT3 _axis);
};
#endif
