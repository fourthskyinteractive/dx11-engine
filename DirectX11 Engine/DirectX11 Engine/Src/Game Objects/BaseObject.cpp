#include "BaseObject.h"
#include "../Renderer/D3D11Renderer.h"
#include "../Game/Game.h"

BaseObject::BaseObject()
{

}

BaseObject::BaseObject(const BaseObject& _baseObject)
{

}

BaseObject::~BaseObject()
{

}

void BaseObject::Initialize(ObjectType _objectType)
{
	if(_objectType == OBJECT_CUBE)
	{
		CreateCube(POS_COLOR);
	}
	else if(_objectType == OBJECT_CUBE_N)
	{
		//Pass in the vertex structure with normals as well.
	}
}

void BaseObject::Render()
{
	D3D11Renderer::d3dImmediateContext->DrawIndexed(
	shaderVariables.indexCount,
	0,
	0);
}

void BaseObject::Remove()
{

}

void BaseObject::AddTexture(WCHAR* _filename)
{

}

void BaseObject::CreateCube(VertexStructure _vertexStructure)
{
	shaderVariables.vertexSize = sizeof(SimpleCubeVertex);
	shaderVariables.vertexShader = BASIC_VERTEX_SHADER;
	shaderVariables.pixelShader = BASIC_PIXEL_SHADER;

	HRESULT hr;

	const D3D11_INPUT_ELEMENT_DESC basicVertexLayoutDesc[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = D3D11Renderer::d3dDevice->CreateInputLayout(
		basicVertexLayoutDesc,
		ARRAYSIZE(basicVertexLayoutDesc),
		ShaderManager::vertexShaders[BASIC_VERTEX_SHADER].buffer->GetBufferPointer(),
		ShaderManager::vertexShaders[BASIC_VERTEX_SHADER].buffer->GetBufferSize(),
		&shaderVariables.inputLayout);

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

	shaderVariables.indexCount = 36;

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
		&shaderVariables.vertexBuffer);

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
		&shaderVariables.indexBuffer);

	int size = sizeof(ConstantBuffer);
	D3D11_BUFFER_DESC constantBufferDesc = {0};
	constantBufferDesc.ByteWidth = sizeof(shaderVariables.constantBufferData);
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	hr = D3D11Renderer::d3dDevice->CreateBuffer(
		&constantBufferDesc,
		nullptr,
		&shaderVariables.constantBuffer);
}

void BaseObject::SetWorldMatrix()
{
	XMMATRIX mScale;
	XMMATRIX mTranslation;
	XMMATRIX mRotation;

	mScale = XMMatrixScaling(objectVariables.scale.x, objectVariables.scale.y, objectVariables.scale.z);
	mTranslation = XMMatrixTranslation(objectVariables.position.x, objectVariables.position.y, objectVariables.position.z);

	objectVariables.rotation.x = XMConvertToRadians(objectVariables.rotation.x);
	objectVariables.rotation.y = XMConvertToRadians(objectVariables.rotation.y);
	objectVariables.rotation.z = XMConvertToRadians(objectVariables.rotation.z);

	mRotation = XMMatrixIdentity();
	if(objectVariables.rotation.x != 0)
	{
		XMFLOAT3 axis = XMFLOAT3(1.0f, 0.0f, 0.0f);
		mRotation *= XMMatrixRotationAxis(XMLoadFloat3(&axis), objectVariables.rotation.x);
	}
	if(objectVariables.rotation.y != 0)
	{
		XMFLOAT3 axis = XMFLOAT3(0.0f, 1.0f, 0.0f);
		mRotation *= XMMatrixRotationAxis(XMLoadFloat3(&axis), objectVariables.rotation.y);
	}
	if(objectVariables.rotation.z != 0)
	{
		XMFLOAT3 axis = XMFLOAT3(0.0f, 0.0f, 1.0f);
		mRotation *= XMMatrixRotationAxis(XMLoadFloat3(&axis), objectVariables.rotation.z);
	}

	objectVariables.worldMatrix = mScale * mRotation * mTranslation;
	objectVariables.worldMatrix = XMMatrixTranspose(objectVariables.worldMatrix);
}

void BaseObject::SetConstantBuffer()
{
	XMStoreFloat4x4(&shaderVariables.constantBufferData.viewProjection, XMMatrixTranspose(Game::camera->GetViewProjectionMatrix()));
	XMStoreFloat4x4(&shaderVariables.constantBufferData.world, objectVariables.worldMatrix);
}

void BaseObject::UpdateConstantBuffer()
{
	D3D11Renderer::d3dImmediateContext->UpdateSubresource(
		shaderVariables.constantBuffer,
		0,
		nullptr,
		&shaderVariables.constantBufferData,
		0,
		0
		);
}

void BaseObject::SetRendererParameters()
{
	D3D11Renderer::d3dImmediateContext->IASetInputLayout(shaderVariables.inputLayout);

	UINT stride = sizeof(Pos_Color_Vertex);
	UINT offset = 0;

	D3D11Renderer::d3dImmediateContext->IASetVertexBuffers(
		0,
		1,
		&shaderVariables.vertexBuffer,
		&stride,
		&offset);

	D3D11Renderer::d3dImmediateContext->IASetIndexBuffer(
		shaderVariables.indexBuffer,
		DXGI_FORMAT_R16_UINT,
		0);

	D3D11Renderer::d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D11Renderer::d3dImmediateContext->VSSetShader(
		ShaderManager::vertexShaders[shaderVariables.vertexShader].shader,
		nullptr,
		0);

	D3D11Renderer::d3dImmediateContext->VSSetConstantBuffers(
		0,
		1,
		&shaderVariables.constantBuffer);

	D3D11Renderer::d3dImmediateContext->PSSetShader(
		ShaderManager::pixelShaders[shaderVariables.pixelShader].shader,
		nullptr,
		0);
}