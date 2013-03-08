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

void BaseObject::Remove()
{

}

void BaseObject::AddTexture(WCHAR* _filename)
{

}

void BaseObject::CreateCube(VertexStructure _vertexStructure)
{
	vertexSize = sizeof(World_ViewProj_ConstantBuffer);
	vertexShader = BASIC_VERTEX_SHADER;
	pixelShader = BASIC_PIXEL_SHADER;

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
		&inputLayout);

	Pos_Color_Vertex cubeVertices[] =
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

	indexCount = 36;

	D3D11_BUFFER_DESC vertexBufferDesc = {0};
	vertexBufferDesc.ByteWidth = sizeof(Pos_Color_Vertex) * ARRAYSIZE(cubeVertices);
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

	int size = sizeof(ConstantBuffer);
	D3D11_BUFFER_DESC constantBufferDesc = {0};
	constantBufferDesc.ByteWidth = sizeof(constantBufferData);
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	hr = D3D11Renderer::d3dDevice->CreateBuffer(
		&constantBufferDesc,
		nullptr,
		&constantBuffer);
}

void BaseObject::SetWorldMatrix(XMFLOAT3 _pos, float _scale, XMFLOAT3 _rotationDegrees)
{
	XMMATRIX scale;
	XMMATRIX translation;
	XMMATRIX rotation;

	scale = XMMatrixScaling(_scale, _scale, _scale);
	translation = XMMatrixTranslation(_pos.x, _pos.y, _pos.z);

	_rotationDegrees.x = XMConvertToRadians(_rotationDegrees.x);
	_rotationDegrees.y = XMConvertToRadians(_rotationDegrees.y);
	_rotationDegrees.z = XMConvertToRadians(_rotationDegrees.z);

	rotation = XMMatrixIdentity();
	if(_rotationDegrees.x != 0)
	{
		XMFLOAT3 axis = XMFLOAT3(1.0f, 0.0f, 0.0f);
		rotation *= XMMatrixRotationAxis(XMLoadFloat3(&axis), _rotationDegrees.x);
	}
	if(_rotationDegrees.y != 0)
	{
		XMFLOAT3 axis = XMFLOAT3(0.0f, 1.0f, 0.0f);
		rotation *= XMMatrixRotationAxis(XMLoadFloat3(&axis), _rotationDegrees.y);
	}
	if(_rotationDegrees.z != 0)
	{
		XMFLOAT3 axis = XMFLOAT3(0.0f, 0.0f, 1.0f);
		rotation *= XMMatrixRotationAxis(XMLoadFloat3(&axis), _rotationDegrees.z);
	}

	worldMatrix = scale * rotation * translation;
	worldMatrix = XMMatrixTranspose(worldMatrix);
}

void BaseObject::SetConstantBuffer()
{
	XMStoreFloat4x4(&constantBufferData.viewProjection, XMMatrixTranspose(Game::camera->GetViewProjectionMatrix()));
	XMStoreFloat4x4(&constantBufferData.world, worldMatrix);
}

void BaseObject::SetRendererParameters()
{
	D3D11Renderer::d3dImmediateContext->UpdateSubresource(
		constantBuffer,
		0,
		nullptr,
		&constantBufferData,
		0,
		0
		);

	D3D11Renderer::d3dImmediateContext->IASetInputLayout(inputLayout);

	UINT stride = vertexSize;
	UINT offset = 0;

	D3D11Renderer::d3dImmediateContext->IASetVertexBuffers(
		0,
		1,
		&vertexBuffer,
		&stride,
		&offset);

	D3D11Renderer::d3dImmediateContext->IASetIndexBuffer(
		indexBuffer,
		DXGI_FORMAT_R16_UINT,
		0);

	D3D11Renderer::d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D11Renderer::d3dImmediateContext->VSSetShader(
		ShaderManager::vertexShaders[vertexShader].shader,
		nullptr,
		0);

	D3D11Renderer::d3dImmediateContext->VSSetConstantBuffers(
		0,
		1,
		&constantBuffer);

	D3D11Renderer::d3dImmediateContext->PSSetShader(
		ShaderManager::pixelShaders[pixelShader].shader,
		nullptr,
		0);
	D3D11Renderer::d3dImmediateContext->DrawIndexed(
		indexCount,
		0,
		0);
}

	//D3D11Renderer::ClearScene(reinterpret_cast<const float*>(&XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));

	//D3D11Renderer::d3dImmediateContext->UpdateSubresource(
	//	cubeObj.constantBuffer,
	//	0,
	//	nullptr,
	//	&cubeObj.constantBufferData,
	//	0,
	//	0
	//	);

	//D3D11Renderer::d3dImmediateContext->IASetInputLayout(cubeObj.inputLayout);

	//UINT stride = cubeObj.vertexSize;
	//UINT offset = 0;

	//D3D11Renderer::d3dImmediateContext->IASetVertexBuffers(
	//	0,
	//	1,
	//	&cubeObj.vertexBuffer,
	//	&stride,
	//	&offset);

	//D3D11Renderer::d3dImmediateContext->IASetIndexBuffer(
	//	cubeObj.indexBuffer,
	//	DXGI_FORMAT_R16_UINT,
	//	0);

	//D3D11Renderer::d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//D3D11Renderer::d3dImmediateContext->VSSetShader(
	//	ShaderManager::vertexShaders[cubeObj.vertexShader].shader,
	//	nullptr,
	//	0);

	//D3D11Renderer::d3dImmediateContext->VSSetConstantBuffers(
	//	0,
	//	1,
	//	&cubeObj.constantBuffer);

	//D3D11Renderer::d3dImmediateContext->PSSetShader(
	//	ShaderManager::pixelShaders[cubeObj.pixelShader].shader,
	//	nullptr,
	//	0);
	//D3D11Renderer::d3dImmediateContext->DrawIndexed(
	//	cubeObj.indexCount,
	//	0,
	//	0);

	//D3D11Renderer::Present(1, 0);