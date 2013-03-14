#include "CubeObjectTexture.h"
#include "../Renderer/D3D11Renderer.h"
#include "../Game/Game.h"

#include <fstream>
using namespace std;


CubeObjectTexture::CubeObjectTexture()
{
	vertexBuffer = NULL;
	indexbuffer = NULL;
	textures = new TextureManager();
	model = NULL;
}

CubeObjectTexture::CubeObjectTexture(const CubeObjectTexture& _cubeObjectTexture)
{

}

CubeObjectTexture::~CubeObjectTexture()
{

}

bool CubeObjectTexture::Initialize(XMFLOAT3 _pos, XMFLOAT3 _scale, XMFLOAT3 _rotation, char* _modelFilename, WCHAR* _textureFilename)
{
	bool result;
	result = LoadModel(_modelFilename);
	if(!result) 
	{
		return false;
	}

	shaderUsed.Initialize();
	result = InitializeBuffers();
	if(!result)
	{
		return false;
	}

	int textureIndex = textures->AddTexture(D3D11Renderer::d3dDevice, _textureFilename);
	if(!textureIndex)
	{
		return false;
	}

	return true;
}

void CubeObjectTexture::Shutdown()
{
	ReleaseTextures();

	ShutdownBuffers();

	ReleaseModel();
}

void CubeObjectTexture::Render()
{
	RenderBuffers();
}

bool CubeObjectTexture::InitializeBuffers()
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT hr;

	//Create the vertex array
	vertices = new VertexType[vertexCount];
	if(!vertices)
	{
		return false;
	}

	//Create the index array
	indices = new unsigned long[indexCount];
	if(!indices)
	{
		return false;
	}

	//Load the vertex array and index array with data
	for(int i = 0; i < vertexCount; ++i)
	{
		vertices[i].position = XMFLOAT3(model[i].x, model[i].y, model[i].z);
		vertices[i].texture = XMFLOAT2(model[i].tu, model[i].tv);
		vertices[i].normal = XMFLOAT3(model[i].nx, model[i].ny, model[i].nz);

		indices[i] = i;
	}

	//Set up the description of the static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Now create the vertex buffer
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
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	hr = D3D11Renderer::d3dDevice->CreateBuffer(&indexBufferDesc, &indexData, &indexbuffer);
	if(FAILED(hr))
	{
		return false;
	}

	//Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

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
	return true;
}

void CubeObjectTexture::UpdateWorldMatrix(XMFLOAT3 _pos, XMFLOAT3 _scale, XMFLOAT3 _rotation)
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

void CubeObjectTexture::ShutdownBuffers()
{
	//Release the index buffer
	if(indexbuffer)
	{
		indexbuffer->Release();
		indexbuffer = 0;
	}

	if(vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = 0;
	}
}

void CubeObjectTexture::RenderBuffers()
{
	XMFLOAT4X4 viewProj;
	XMStoreFloat4x4(&viewProj, Game::camera->GetViewProjectionMatrix());

	shaderUsed.Render(worldMatrix, viewProj, textures->GetTexture(0), Game::lightDiffuse->GetDirection(), Game::lightDiffuse->GetDiffuseColor(), Game::lightDiffuse->GetAmbientColor(), indexCount);
}

void CubeObjectTexture::ReleaseTextures()
{
	textures->Shutdown();
}

bool CubeObjectTexture::LoadModel(char* _filePath)
{
	ifstream fin;
	char input;
	int i;


	// Open the model file.
	fin.open(_filePath);

	// If it could not open the file then exit.
	if(fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> vertexCount;

	// Set the number of indices to be the same as the vertex count.
	indexCount = vertexCount;

	// Create the model using the vertex count that was read in.
	model = new ModelType[vertexCount];
	if(!model)
	{
		return false;
	}

	// Read up to the beginning of the data.
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for(i=0; i<vertexCount; i++)
	{
		fin >> model[i].x >> model[i].y >> model[i].z;
		fin >> model[i].tu >> model[i].tv;
		fin >> model[i].nx >> model[i].ny >> model[i].nz;
	}

	// Close the model file.
	fin.close();

	return true;
}

void CubeObjectTexture::ReleaseModel()
{
	if(model)
	{
		delete[] model;
		model = 0;
	}
}