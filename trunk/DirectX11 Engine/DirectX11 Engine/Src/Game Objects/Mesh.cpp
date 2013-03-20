#include "mesh.h"
#include "../Renderer/D3D11Renderer.h"
#include "../Game/Game.h"

#include <fstream>
using namespace std;


Mesh::Mesh()
{
	vertexBuffer = NULL;
	indexbuffer = NULL;
	textures = new TextureManager();
	mesh = NULL;
}

Mesh::Mesh(const Mesh& _mesh)
{

}

Mesh::~Mesh()
{

}

bool Mesh::Initialize(XMFLOAT3 _pos, XMFLOAT3 _scale, XMFLOAT3 _rotation, char* _meshFilename)
{
	bool result;
	result = LoadMesh(_meshFilename);
	if(!result) 
	{
		return false;
	}

	shaderUsed.Initialize();
	CalculateMeshVectors();
	result = InitializeBuffers();
	if(!result)
	{
		return false;
	}

	return true;
}

void Mesh::AddTexture(WCHAR* _filePath)
{
	textures->AddTexture(D3D11Renderer::d3dDevice, _filePath);
}

void Mesh::Shutdown()
{
	ReleaseTextures();

	ShutdownBuffers();

	ReleaseMesh();
}

void Mesh::Render()
{
	RenderBuffers();
}

bool Mesh::InitializeBuffers()
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
		vertices[i].position = XMFLOAT4(mesh[i].x, mesh[i].y, mesh[i].z, mesh[i].w);
		vertices[i].texture = XMFLOAT2(mesh[i].tu, mesh[i].tv);
		vertices[i].normal = XMFLOAT3(mesh[i].nx, mesh[i].ny, mesh[i].nz);
		vertices[i].tangent = XMFLOAT3(mesh[i].tx, mesh[i].ty, mesh[i].tz);
		vertices[i].binormal = XMFLOAT3(mesh[i].bx, mesh[i].by, mesh[i].bz);

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

void Mesh::UpdateWorldMatrix(XMFLOAT3 _pos, XMFLOAT3 _scale, XMFLOAT3 _rotation)
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

void Mesh::ShutdownBuffers()
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

void Mesh::RenderBuffers()
{
	XMFLOAT4X4 viewProj;
	XMStoreFloat4x4(&viewProj, Game::camera->GetViewProjectionMatrix());

	shaderUsed.Render(worldMatrix, viewProj, (ID3D11ShaderResourceView**)textures->GetTextureArrayPointer(), Game::lightDiffuse->GetDirection(), Game::lightDiffuse->GetDiffuseColor(), Game::lightDiffuse->GetAmbientColor(), indexCount);
}

void Mesh::ReleaseTextures()
{
	textures->Shutdown();
}

bool Mesh::LoadMesh(char* _filePath)
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
	mesh = new MeshType[vertexCount];
	if(!mesh)
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
		fin >> mesh[i].x >> mesh[i].y >> mesh[i].z >> mesh[i].w;
		fin >> mesh[i].tu >> mesh[i].tv;
		fin >> mesh[i].nx >> mesh[i].ny >> mesh[i].nz;
	}

	// Close the model file.
	fin.close();

 	return true;
}

void Mesh::CalculateMeshVectors()
{
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal, normal;


	// Calculate the number of faces in the model.
	faceCount = vertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for(i=0; i<faceCount; i++)
	{
		// Get the three vertices for this face from the model.
		vertex1.x = mesh[index].x;
		vertex1.y = mesh[index].y;
		vertex1.z = mesh[index].z;
		vertex1.tu = mesh[index].tu;
		vertex1.tv = mesh[index].tv;
		vertex1.nx = mesh[index].nx;
		vertex1.ny = mesh[index].ny;
		vertex1.nz = mesh[index].nz;
		index++;

		vertex2.x = mesh[index].x;
		vertex2.y = mesh[index].y;
		vertex2.z = mesh[index].z;
		vertex2.tu = mesh[index].tu;
		vertex2.tv = mesh[index].tv;
		vertex2.nx = mesh[index].nx;
		vertex2.ny = mesh[index].ny;
		vertex2.nz = mesh[index].nz;
		index++;

		vertex3.x = mesh[index].x;
		vertex3.y = mesh[index].y;
		vertex3.z = mesh[index].z;
		vertex3.tu = mesh[index].tu;
		vertex3.tv = mesh[index].tv;
		vertex3.nx = mesh[index].nx;
		vertex3.ny = mesh[index].ny;
		vertex3.nz = mesh[index].nz;
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Calculate the new normal using the tangent and binormal.
		CalculateNormal(tangent, binormal, normal);

		// Store the normal, tangent, and binormal for this face back in the mesh structure.
		mesh[index-1].nx = normal.x;
		mesh[index-1].ny = normal.y;
		mesh[index-1].nz = normal.z;
		mesh[index-1].tx = tangent.x;
		mesh[index-1].ty = tangent.y;
		mesh[index-1].tz = tangent.z;
		mesh[index-1].bx = binormal.x;
		mesh[index-1].by = binormal.y;
		mesh[index-1].bz = binormal.z;

		mesh[index-2].nx = normal.x;
		mesh[index-2].ny = normal.y;
		mesh[index-2].nz = normal.z;
		mesh[index-2].tx = tangent.x;
		mesh[index-2].ty = tangent.y;
		mesh[index-2].tz = tangent.z;
		mesh[index-2].bx = binormal.x;
		mesh[index-2].by = binormal.y;
		mesh[index-2].bz = binormal.z;

		mesh[index-3].nx = normal.x;
		mesh[index-3].ny = normal.y;
		mesh[index-3].nz = normal.z;
		mesh[index-3].tx = tangent.x;
		mesh[index-3].ty = tangent.y;
		mesh[index-3].tz = tangent.z;
		mesh[index-3].bx = binormal.x;
		mesh[index-3].by = binormal.y;
		mesh[index-3].bz = binormal.z;
	}

	return;
}

void Mesh::CalculateTangentBinormal(TempVertexType _vertex1, TempVertexType _vertex2, TempVertexType _vertex3, VectorType& tangent, VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;


	// Calculate the two vectors for this face.
	vector1[0] = _vertex2.x - _vertex1.x;
	vector1[1] = _vertex2.y - _vertex1.y;
	vector1[2] = _vertex2.z - _vertex1.z;

	vector2[0] = _vertex3.x - _vertex1.x;
	vector2[1] = _vertex3.y - _vertex1.y;
	vector2[2] = _vertex3.z - _vertex1.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = _vertex2.tu - _vertex1.tu;
	tvVector[0] = _vertex2.tv - _vertex1.tv;

	tuVector[1] = _vertex3.tu - _vertex1.tu;
	tvVector[1] = _vertex3.tv - _vertex1.tv;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of this normal.
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the normal and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of this normal.
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the normal and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
}

void Mesh::CalculateNormal(VectorType _tangent, VectorType _binormal, VectorType& _normal)
{
	float length;

	// Calculate the cross product of the _tangent and _bi_normal which will give the normal vector.
	_normal.x = (_tangent.y * _binormal.z) - (_tangent.z * _binormal.y);
	_normal.y = (_tangent.z * _binormal.x) - (_tangent.x * _binormal.z);
	_normal.z = (_tangent.x * _binormal.y) - (_tangent.y * _binormal.x);

	// Calculate the length of the normal.
	length = sqrt((_normal.x * _normal.x) + (_normal.y * _normal.y) + (_normal.z * _normal.z));

	// Normalize the normal.
	_normal.x = _normal.x / length;
	_normal.y = _normal.y / length;
	_normal.z = _normal.z / length;

	return;
}

void Mesh::ReleaseMesh()
{
	if(mesh)
	{
		delete[] mesh;
		mesh = 0;
	}
}