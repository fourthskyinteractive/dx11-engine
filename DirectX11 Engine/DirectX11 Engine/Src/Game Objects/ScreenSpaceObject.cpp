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

void ScreenSpaceObject::Initialize(ID3D11RenderTargetView* _renderTargetView, ID3D11ShaderResourceView* _shaderResourceView)
{
	renderTarget = _renderTargetView;
	for(int i = 0; i < 4; ++i)
	{
		textures[i] = D3D11Renderer::shaderResourceView[i + 1];
	}

	shaderUsed.Initialize();
	shaderUsed.UpdatePixelShaderTextureConstants(textures);
}

void ScreenSpaceObject::SetShaderBuffers()
{
	//Set the vertex buffer to active in the input assembler so it can be rendered
	//D3D11Renderer::d3dImmediateContext->IASetVertexBuffers(0, 0, NULL, 0, 0);

	//Set the index buffer to active in the input assembler so it can be rendered
	//D3D11Renderer::d3dImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive that should be rendered from this vertex buffer, in this case triangles
	D3D11Renderer::d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
}

void ScreenSpaceObject::Render()
{	
	SetShaderBuffers();
	Update();
	shaderUsed.Render(1, renderTarget);
}

void ScreenSpaceObject::Update()
{
	shaderUsed.Update(NULL, textures);
}

void ScreenSpaceObject::ChangeShaderResourceView(ID3D11ShaderResourceView* _shaderResourceView[])
{
// 	texture = NULL;
// 	texture = _shaderResourceView;
}