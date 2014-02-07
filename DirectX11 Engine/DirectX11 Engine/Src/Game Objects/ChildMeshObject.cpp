#include "ChildMeshObject.h"

#include "ParentMeshObject.h"
#include "../Utility/Model Loaders/FBXLoader.h"
#include "../Game/Game.h"
#include "../Renderer/D3D11Renderer.h"

ChildMeshObject::ChildMeshObject()
{
	textures = new TextureManager();
}

ChildMeshObject::ChildMeshObject(const ChildMeshObject& _childMeshObject)
{

}

ChildMeshObject::~ChildMeshObject()
{

}

void ChildMeshObject::Initialize(XMFLOAT3 _position, XMFLOAT3 _scale, XMFLOAT3 _rotation)
{
	position = _position;
	scale = _scale;
	rotation = _rotation;
	UpdateLocalMatrix();
	UpdateWorldMatrix();
}

bool ChildMeshObject::InitializeBuffers(VertexType* _vertices, unsigned long* _indices)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	//Set up the description of the static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = _vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Now create the vertex buffer

	HRESULT hr;

	hr = D3D11Renderer::d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if(FAILED(hr))
	{
		return false;
	}

	//Set up the description of the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//Give the subresource structure a pointer to the index data
	indexData.pSysMem = _indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	hr = D3D11Renderer::d3dDevice->CreateBuffer(&indexBufferDesc, &indexData, &indexbuffer);
	if(FAILED(hr))
	{
		return false;
	}

	//Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] _vertices;
	_vertices = 0;

	delete[] _indices;
	_indices = 0;

	SetShaderBuffers();

	return true;
}

void ChildMeshObject::SetShaderBuffers()
{
	unsigned int stride;
	unsigned int offset;

	//Set the vertex buffer stride and offset
	stride = sizeof(VertexType);
	offset = 0;

	//Set the vertex buffer to active in the input assembler so it can be rendered
	D3D11Renderer::d3dImmediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered
	D3D11Renderer::d3dImmediateContext->IASetIndexBuffer(indexbuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive that should be rendered from this vertex buffer, in this case triangles
	D3D11Renderer::d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ChildMeshObject::AddTexture(WCHAR* _filePath)
{
	textures->AddTexture(_filePath);
}

void ChildMeshObject::Render(BaseShader* _shaderUsed, ID3D11RenderTargetView* _renderTarget)
{	
	SetShaderBuffers();

	_shaderUsed->Render(indexCount, _renderTarget);
}

void ChildMeshObject::Update(float _dt)
{
	//TODO: NEED TO APPLY THE CAMERA ROTATE SYSTEM TO THIS BECAUSE IT MAINTAINS A GLOBAL COORDINATE SYSTEM
// 	float degrees = 45.0f * _dt;
// 
// 	Rotate(degrees, XMFLOAT3(0.0f, 1.0f, 0.0f));

	UpdateLocalMatrix();

	UpdateWorldMatrix();
}

void ChildMeshObject::Rotate(float _degrees, XMFLOAT3 _axis)
{
	_degrees = XMConvertToRadians(_degrees);

	rotation.x += _axis.x * _degrees;
	rotation.y += _axis.y * _degrees;
	rotation.z += _axis.z * _degrees;
}

void ChildMeshObject::UpdateLocalMatrix()
{
	XMMATRIX mScale;
	XMMATRIX mTranslation;
	XMMATRIX mRotation;

	mScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	mTranslation = XMMatrixTranslation(position.x, position.y, position.z);

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