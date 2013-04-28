#include "LightObjects.h"

#include "ParentMeshObject.h"
#include "../Utility/Model Loaders/FBXLoader.h"
#include "../Game/Game.h"
#include "../Renderer/D3D11Renderer.h"


ID3D11Buffer*				LightObjects::vertexBuffer;
ID3D11Buffer*				LightObjects::indexbuffer;

ID3D11ShaderResourceView*	LightObjects::texture;
ID3D11RenderTargetView*		LightObjects::renderTarget;

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
	//shaderUsed.Initialize();
	//shaderUsed.UpdatePixelShaderTextureConstants(texture);
}

void LightObjects::SetShaderBuffers()
{
	unsigned int stride;
	unsigned int offset;

	//Set the vertex buffer stride and offset
	stride = sizeof(Pos_Tex_Vertex);
	offset = 0;

	D3D11Renderer::d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
}

void LightObjects::Render()
{	
	SetShaderBuffers();
	Update();
	//shaderUsed.Render(1, renderTarget);
}

void LightObjects::Update()
{
	//shaderUsed.Update(NULL, texture);
}

void LightObjects::ChangeShaderResourceView(ID3D11ShaderResourceView* _shaderResourceView)
{
	texture = NULL;
	texture = _shaderResourceView;
}