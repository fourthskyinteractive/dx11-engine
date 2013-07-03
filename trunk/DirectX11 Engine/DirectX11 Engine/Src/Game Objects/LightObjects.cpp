#include "LightObjects.h"

#include "ParentMeshObject.h"
#include "../Utility/Model Loaders/FBXLoader.h"
#include "../Game/Game.h"
#include "../Renderer/D3D11Renderer.h"
#include "../Utility/Geometry Makers/GeometryMaker.h"


ID3D11Buffer*				LightObjects::vertexBuffer;
ID3D11Buffer*				LightObjects::indexbuffer;

ID3D11ShaderResourceView*	LightObjects::texture;
ID3D11RenderTargetView*		LightObjects::renderTarget;
ObjectShader				LightObjects::shaderUsed;
vector<XMFLOAT3*>			LightObjects::lightVerts;
vector<unsigned long*>		LightObjects::lightIndices;
int							LightObjects::vertexCount;
int							LightObjects::indexCount;

LightObjects::LightObjects()
{

}

LightObjects::LightObjects(const LightObjects& _childMeshObject)
{

}

LightObjects::~LightObjects()
{

}

void LightObjects::Initialize(ID3D11RenderTargetView* _renderTargetView, ID3D11ShaderResourceView* _shaderResourceView)
{
	renderTarget = _renderTargetView;
	texture = _shaderResourceView;
	InitializeBuffers();
	//shaderUsed.Initialize();
	shaderUsed.UpdatePixelShaderTextureConstants(texture);
}

void LightObjects::InitializeBuffers()
{
	XMFLOAT3* vertices = NULL;
	unsigned long* indices = NULL;

	GeometryMaker::CreateSphere(20, 2.0f, &vertices, &indices, &vertexCount, &indexCount);

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	//Set up the description of the static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(XMFLOAT3) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
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
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//Give the subresource structure a pointer to the index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	hr = D3D11Renderer::d3dDevice->CreateBuffer(&indexBufferDesc, &indexData, &indexbuffer);
	if(FAILED(hr))
	{
		return;
	}

	//Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	SetShaderBuffers();
}

void LightObjects::SetShaderBuffers()
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

void LightObjects::Render()
{	
	SetShaderBuffers();
	Update();
	shaderUsed.Render(indexCount, renderTarget);
}

void LightObjects::Update()
{
	shaderUsed.Update(NULL, texture);
}

void LightObjects::ChangeShaderResourceView(ID3D11ShaderResourceView* _shaderResourceView)
{
	texture = NULL;
	texture = _shaderResourceView;
}