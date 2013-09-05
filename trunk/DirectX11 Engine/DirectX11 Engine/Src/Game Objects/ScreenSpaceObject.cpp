#include "ScreenSpaceObject.h"

#include "ParentMeshObject.h"
#include "../Utility/Model Loaders/FBXLoader.h"
#include "../Game/Game.h"
#include "../Renderer/D3D11Renderer.h"

ScreenSpaceObject::ScreenSpaceObject()
{

}

ScreenSpaceObject::ScreenSpaceObject(const ScreenSpaceObject& _childMeshObject)
{

}

ScreenSpaceObject::~ScreenSpaceObject()
{

}

void ScreenSpaceObject::Initialize(ID3D11RenderTargetView* _renderTargetView, ID3D11ShaderResourceView* _shaderResourceView, VERTEX_SHADERS _vertexShader, PIXEL_SHADERS _pixelShader, GEOMETRY_SHADERS _geometryShader)
{
	renderTarget = _renderTargetView;
	for(int i = 0; i < 4; ++i)
	{
		textures[i] = D3D11Renderer::shaderResourceView[i + 1];
	}
	InitializeBuffers(NULL, NULL);
	shaderUsed.Initialize(_vertexShader, _pixelShader, _geometryShader);
	shaderUsed.UpdatePixelShaderTextureConstants(textures);
}

bool ScreenSpaceObject::InitializeBuffers(VertexType* _vertices, unsigned long* _indices)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	XMFLOAT3 dummyValue = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//Set up the description of the static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(XMFLOAT3);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = &dummyValue;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Now create the vertex buffer

	HRESULT hr;

	hr = D3D11Renderer::d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if(FAILED(hr))
	{
		return false;
	}

	return true;
}

void ScreenSpaceObject::SetShaderBuffers()
{
	unsigned int stride;
	unsigned int offset;

	//Set the vertex buffer stride and offset
	stride = sizeof(VertexType);
	offset = 0;

	//Set the vertex buffer to active in the input assembler so it can be rendered
	D3D11Renderer::d3dImmediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered
	//D3D11Renderer::d3dImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive that should be rendered from this vertex buffer, in this case triangles
	D3D11Renderer::d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
}

void ScreenSpaceObject::Render()
{
	SetShaderBuffers();
	int numPointLights = LightManager::GetNumberPointLights();
	int numDirectionalLights = LightManager::GetNumberDirectionalLights();

	shaderUsed.UpdatePerFrame(renderTarget, textures);

	for(int i = 0; i < numPointLights; ++i)
	{
		Update(POINT_LIGHT, i);
		shaderUsed.Render(1);
	}

	for(int i = 0; i < numDirectionalLights; ++i)
	{
		Update(DIRCTIONAL_LIGHT, i);
		shaderUsed.Render(1);
	}

	if(LightManager::GetAmbientLight())
	{
		Update(AMBIENT_LIGHT, 0);
		shaderUsed.Render(1);
	}
}

void ScreenSpaceObject::Update(LIGHT_TYPE _lightType, int _lightIndex)
{
	shaderUsed.UpdatePerLight(_lightType, _lightIndex);
}

void ScreenSpaceObject::ChangeShaderResourceView(ID3D11ShaderResourceView* _shaderResourceView[])
{
// 	texture = NULL;
// 	texture = _shaderResourceView;
}