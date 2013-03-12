#include "CubeObjectColor.h"
#include "../Renderer/D3D11Renderer.h"
#include "../Camera/Camera.h"
#include "../Game/Game.h"

CubeObjectColor::CubeObjectColor()
{
	vertexBuffer = NULL;
	indexBuffer = NULL;
}

CubeObjectColor::CubeObjectColor(const CubeObjectColor&)
{

}

CubeObjectColor::~CubeObjectColor()
{

}

bool CubeObjectColor::Initialize(ID3D11Device* _device, XMFLOAT3 _pos, XMFLOAT3 _scale, XMFLOAT3 _rotation)
{
	UpdateWorldMatrix(_pos, _scale, _rotation);

	bool result;
	shaderUsed.Initialize();

	result = InitializeBuffers(_device);
	if(!result)
	{
		return false;
	}

	return true;
}

void CubeObjectColor::Update(float _dt)
{
	
	//XMFLOAT4X4 viewProj;
	//XMStoreFloat4x4(&viewProj, Game::camera->GetViewProjectionMatrix());
	//shaderUsed.UpdateShaderConstants(D3D11Renderer::d3dImmediateContext, worldMatrix, viewProj);
}

void CubeObjectColor::UpdateWorldMatrix(XMFLOAT3 _pos, XMFLOAT3 _scale, XMFLOAT3 _rotation)
{
	XMMATRIX mScale;
	XMMATRIX mTranslation;
	XMMATRIX mRotation;

	mScale = XMMatrixScaling(_scale.x, _scale.y, _scale.z);
	mTranslation = XMMatrixTranslation(_pos.x, _pos.y, _pos.z);

	_rotation.x = XMConvertToRadians(_rotation.x);
	_rotation.y = XMConvertToRadians(_rotation.y);
	_rotation.z = XMConvertToRadians(_rotation.z);

	mRotation = XMMatrixIdentity();
	if(_rotation.x != 0)
	{
		XMFLOAT3 axis = XMFLOAT3(1.0f, 0.0f, 0.0f);
		mRotation *= XMMatrixRotationAxis(XMLoadFloat3(&axis), _rotation.x);
	}
	if(_rotation.y != 0)
	{
		XMFLOAT3 axis = XMFLOAT3(0.0f, 1.0f, 0.0f);
		mRotation *= XMMatrixRotationAxis(XMLoadFloat3(&axis), _rotation.y);
	}
	if(_rotation.z != 0)
	{
		XMFLOAT3 axis = XMFLOAT3(0.0f, 0.0f, 1.0f);
		mRotation *= XMMatrixRotationAxis(XMLoadFloat3(&axis), _rotation.z);
	}

	XMMATRIX mWorldMat = XMMatrixIdentity();
	mWorldMat *= mScale;
	mWorldMat *= mRotation;
	mWorldMat *= mTranslation;

	XMStoreFloat4x4(&worldMatrix, mWorldMat);
}

void CubeObjectColor::Shutdown()
{
	ShutdownBuffers();
}

void CubeObjectColor::Render(ID3D11DeviceContext* _deviceContext)
{
	RenderBuffers(_deviceContext);
}

int CubeObjectColor::GetIndeCount()
{
	return indexCount;
}

bool CubeObjectColor::InitializeBuffers(ID3D11Device* _device)
{
	HRESULT hr;

	SimpleCubeVertex cubeVertices[] =
	{
		{ XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) }, // +Y (top face)
		{ XMFLOAT3( 0.5f, 0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
		{ XMFLOAT3( 0.5f, 0.5f,  0.5f), XMFLOAT3(.50f, 1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, 0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f) },

		{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) }, // -Y (bottom face)
		{ XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
	};

	unsigned short cubeIndices[] =
	{
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		3, 2, 5,
		3, 5, 4,

		2, 1, 6,
		2, 6, 5,

		1, 7, 6,
		1, 0, 7,

		0, 3, 4,
		0, 4, 7
	};

	indexCount = ARRAYSIZE(cubeIndices);
	vertexCount = ARRAYSIZE(cubeVertices);

	D3D11_BUFFER_DESC vertexBufferDesc = {0};
	vertexBufferDesc.ByteWidth = sizeof(SimpleCubeVertex) * ARRAYSIZE(cubeVertices);
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	vertexBufferData.pSysMem = cubeVertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	hr = D3D11Renderer::d3dDevice->CreateBuffer(
		&vertexBufferDesc,
		&vertexBufferData,
		&vertexBuffer);

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.ByteWidth = sizeof(unsigned short) * ARRAYSIZE(cubeIndices);
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = cubeIndices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	hr = D3D11Renderer::d3dDevice->CreateBuffer(
		&indexBufferDesc,
		&indexBufferData,
		&indexBuffer);

	return true;
}

void CubeObjectColor::ShutdownBuffers()
{
	if(vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = 0;
	}

	if(indexBuffer)
	{
		indexBuffer->Release();
		indexBuffer = 0;
	}
}

void CubeObjectColor::RenderBuffers(ID3D11DeviceContext* _deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	//Set vertex buffer stride and offset
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	_deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMFLOAT4X4 viewProj;
	XMStoreFloat4x4(&viewProj, Game::camera->GetViewProjectionMatrix());

	shaderUsed.Render(indexCount, worldMatrix, viewProj);

	return;
}