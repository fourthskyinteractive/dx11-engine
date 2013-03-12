#include "CubeObjectTexture.h"
#include "../Renderer/D3D11Renderer.h"


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

bool CubeObjectTexture::Initialize(XMFLOAT3 _pos, XMFLOAT3 _scale, XMFLOAT3 _rotation, char* modelFilename, WCHAR* textureFilename)
{
	bool result;
	result = LoadModel(modelFilename);
	if(!result) 
	{
		return false;
	}

	result = InitializeBuffers();
	if(!result)
	{
		return false;
	}

	int textureIndex = textures->AddTexture(D3D11Renderer::d3dDevice, textureFilename);
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

	return true;
}

void CubeObjectTexture::ShutdownBuffers()
{

}

void CubeObjectTexture::RenderBuffers()
{

}

void CubeObjectTexture::ReleaseTextures()
{
	textures->Shutdown();
}

bool CubeObjectTexture::LoadModel(char* _filePath)
{
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