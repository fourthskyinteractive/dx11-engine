#include "ChildMeshObject.h"

#include "ParentMeshObject.h"

#include "../Game/Game.h"

ChildMeshObject::ChildMeshObject()
{

}

ChildMeshObject::ChildMeshObject(const ChildMeshObject& _childMeshObject)
{

}

ChildMeshObject::~ChildMeshObject()
{

}

void ChildMeshObject::Initialize(XMFLOAT3 _position, XMFLOAT3 _scale, XMFLOAT3 _rotation)
{

}

void ChildMeshObject::Render()
{
	XMMATRIX worldMat = XMLoadFloat4x4(&worldMatrix);
	
	//TODO: Create a light manager that houses all of the lights.
	shaderUsed.Render(worldMatrix, 
		Game::camera->GetViewProjectionMatrixF(), 
		(ID3D11ShaderResourceView**)textures->GetTextureArrayPointer(),
		XMFLOAT3(0.0f, 0.0f, 1.0f), 
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4(.15f, .15f, .15f, 1.0f), 
		indexCount);
}

void ChildMeshObject::UpdateLocalMatrix()
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

	XMMATRIX lolcalMat = XMMatrixIdentity();
	lolcalMat *= mScale;
	lolcalMat *= mRotation;
	lolcalMat *= mTranslation;

	XMStoreFloat4x4(&localMatrix, lolcalMat);
}

void ChildMeshObject::UpdateWorldMatrix()
{
	XMStoreFloat4x4(&worldMatrix, ( XMLoadFloat4x4(&localMatrix) * parent->GetWorldMatrixM()));
}