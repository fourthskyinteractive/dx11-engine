#include "Terrain.h"

#include "ParentMeshObject.h"
#include "../Utility/Model Loaders/FBXLoader.h"
#include "../Game/Game.h"
#include "../Renderer/D3D11Renderer.h"
#include "../Utility/Geometry Makers/GeometryMaker.h"
#include "../Utility/Misc/TerrainGenerator.h"


Terrain::Terrain()
{

}

Terrain::Terrain(const Terrain& _childMeshObject)
{

}

Terrain::~Terrain()
{

}

void Terrain::Initialize(ID3D11RenderTargetView* _renderTargetView, ID3D11ShaderResourceView* _shaderResourceView)
{
	renderTarget = _renderTargetView;
	texture = _shaderResourceView;
	InitializeBuffers();
	//shaderUsed.Initialize();
	shaderUsed.UpdatePixelShaderTextureConstants(texture);
}

void Terrain::InitializeBuffers()
{
	vector<XMFLOAT3> verts;
	vector<unsigned long> indices;

	TerrainGenerator::CreateTerrain(1000.0f, 1000.0f, 128, 100.0f, XMFLOAT3(0.0f, -20.0f, 0.0f), verts, indices);

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	//Set up the description of the static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(XMFLOAT3) * verts.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = &verts[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Now create the vertex buffer

	HRESULT hr;

	hr = D3D11Renderer::d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if(FAILED(hr))
	{
		return;
	}

	//Set up the description of the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//Give the subresource structure a pointer to the index data
	indexData.pSysMem = &indices[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	hr = D3D11Renderer::d3dDevice->CreateBuffer(&indexBufferDesc, &indexData, &indexbuffer);
	if(FAILED(hr))
	{
		return;
	}

	SetShaderBuffers();
}

void Terrain::SetShaderBuffers()
{
	unsigned int stride;
	unsigned int offset;

	//Set the vertex buffer stride and offset
	stride = sizeof(XMFLOAT3);
	offset = 0;

	//Set the vertex buffer to active in the input assembler so it can be rendered
	D3D11Renderer::d3dImmediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered
	D3D11Renderer::d3dImmediateContext->IASetIndexBuffer(indexbuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive that should be rendered from this vertex buffer, in this case triangles
	D3D11Renderer::d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Terrain::Render()
{	
	SetShaderBuffers();
	Update();
	shaderUsed.Render(indexCount, renderTarget);
}

void Terrain::Update()
{
	XMMATRIX worldMatM = XMMatrixIdentity();
	XMFLOAT4X4 worldMat;
	XMStoreFloat4x4(&worldMat, worldMatM);
	shaderUsed.Update(worldMat, texture);
}

void Terrain::ChangeShaderResourceView(ID3D11ShaderResourceView* _shaderResourceView)
{
	texture = NULL;
	texture = _shaderResourceView;
}