#include "ParentMeshObject.h"
#include "ChildMeshObject.h"
#include "../Game/Game.h"
#include "../Renderer/D3D11Renderer.h"
#include "../Utility/Model Loaders/FBXLoader.h"
#include "Lights/LightManager.h"


ParentMeshObject::ParentMeshObject()
{
	textures = NULL;
}

ParentMeshObject::ParentMeshObject(const ParentMeshObject& _parentMeshObject)
{

}

ParentMeshObject::~ParentMeshObject()
{

}

void ParentMeshObject::Initialize(char* _filePath, XMFLOAT3 _position, XMFLOAT3 _scale, XMFLOAT3 _rotation, bool _hasOneTexture /* = false */, WCHAR* _textureFilePath /* = L"" */)
{
	position = _position;
	scale = _scale;
	rotation = _rotation;

	textures = new TextureManager();
	if( _hasOneTexture)
	{
		AddTexture(_textureFilePath);
	}

	shaderUsed.Initialize();
	shaderUsed.UpdatePixelShaderTextureConstants(textures->GetTextureArrayPointer());
	shaderUsed.UpdatePixelShaderLightConstants(LightManager::GetDirectionalLight(0)->GetLightDirectionF(), LightManager::GetDirectionalLight(0)->GetLightColorF(), LightManager::GetAmbientLight()->GetLightColorF());
	UpdateWorldMatrix();

	FBXLoader::LoadFBX(this, _filePath);
}


void ParentMeshObject::Render()
{
	for(unsigned int i = 0; i < children.size(); ++i)
	{
		shaderUsed.UpdateVertexShaderConstants(children[i]->GetWorldMatrixF(), Game::camera->GetViewProjectionMatrixF());
		children[i]->Render(&shaderUsed);
	}
}

void ParentMeshObject::AddChild(VertexType* _vertices, unsigned long* _indices, int _numVertices, int _numIdices)
{
	ChildMeshObject* child = new ChildMeshObject();
	child->SetParent(this);
	child->SetIndexCount(_numIdices);
	child->SetVertexCount(_numVertices);
	child->Initialize(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	child->InitializeBuffers(_vertices, _indices);
	children.push_back(child);
}

void ParentMeshObject::AddTexture(WCHAR* _filePath)
{
	textures->AddTexture(D3D11Renderer::d3dDevice, _filePath);
}

void ParentMeshObject::AddChildTexture(WCHAR* _filePath, int _child)
{
	children[_child]->AddTexture(_filePath);
}

void ParentMeshObject::UpdateWorldMatrix()
{
	XMMATRIX mScale;
	XMMATRIX mTranslation;
	XMMATRIX mRotation;

	mScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	mTranslation = XMMatrixTranslation(position.x, position.y, position.z);

	rotation.x = XMConvertToRadians(rotation.x);
	rotation.y = XMConvertToRadians(rotation.y);
	rotation.z = XMConvertToRadians(rotation.z);

	mRotation = XMMatrixIdentity();
	if(rotation.x != 0)
	{
		XMFLOAT3 axis = XMFLOAT3(1.0f, 0.0f, 0.0f);
		mRotation *= XMMatrixRotationAxis(XMLoadFloat3(&axis), rotation.x);
	}
	if(rotation.y != 0)
	{
		XMFLOAT3 axis = XMFLOAT3(0.0f, 1.0f, 0.0f);
		mRotation *= XMMatrixRotationAxis(XMLoadFloat3(&axis), rotation.y);
	}
	if(rotation.z != 0)
	{
		XMFLOAT3 axis = XMFLOAT3(0.0f, 0.0f, 1.0f);
		mRotation *= XMMatrixRotationAxis(XMLoadFloat3(&axis), rotation.z);
	}

	XMMATRIX mWorldMat = XMMatrixIdentity();
	mWorldMat *= mScale;
	mWorldMat *= mRotation;
	mWorldMat *= mTranslation;

	XMStoreFloat4x4(&worldMatrix, mWorldMat);
}