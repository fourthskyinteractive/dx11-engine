#include "ParentMeshObject.h"
#include "ChildMeshObject.h"
#include "../Utility/Model Loaders/FBXLoader.h"


ParentMeshObject::ParentMeshObject()
{

}

ParentMeshObject::ParentMeshObject(const ParentMeshObject& _parentMeshObject)
{

}

ParentMeshObject::~ParentMeshObject()
{

}

void ParentMeshObject::Initialize(char* _filePath, XMFLOAT3 _position, XMFLOAT3 _scale, XMFLOAT3 _rotation)
{
	position = _position;
	scale = _scale;
	rotation = _rotation;

	UpdateWorldMatrix();

	FBXLoader::LoadFBX(this, _filePath);
}

void ParentMeshObject::Render()
{
	for(unsigned int i = 0; i < children.size(); ++i)
	{
		children[i]->Render();
	}
}

void ParentMeshObject::AddChild(VertexType* _vertices, unsigned long* _indices, int _numVertices, int _numIdices)
{
	ChildMeshObject* child = new ChildMeshObject();
	child->SetParent(this);
	child->SetIndexCount(_numIdices);
	child->SetVertexCount(_numVertices);
	child->Initialize(XMFLOAT3(0.0f, 0.0f, 2.0f), XMFLOAT3(.10f, .10f, .10f), XMFLOAT3(0.0f, 90.0f, 90.0f));
	child->InitializeBuffers(_vertices, _indices);
	children.push_back(child);
}

void ParentMeshObject::AddTexture(WCHAR* _filePath, int _child)
{
	for(int i = 0; i < children.size(); ++i)
	{
		children[i]->AddTexture(_filePath);
	}	
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