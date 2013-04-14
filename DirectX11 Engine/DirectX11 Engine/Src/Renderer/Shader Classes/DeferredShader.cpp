#include "DeferredShader.h"
#include "../ShaderManager.h"
#include "../D3D11Renderer.h"
#include "../../Game/Game.h"
#include "../../Game Objects/ChildMeshObject.h"

DeferredShader::DeferredShader()
{
	inputLayout = NULL;
	constantBuffer = NULL;
	SetBufferType(DEPTH_BUFFER);
}

DeferredShader::DeferredShader(const DeferredShader& _depthBuffer)
{

}

DeferredShader::~DeferredShader()
{

}

bool DeferredShader::Initialize()
{
	bool result;

	result = InitializeShader(DEFERRED_COMBINE_VERTEX_SHADER, DEFERRED_COMBINE_PIXEL_SHADER, DEFERRED_COMBINE_GEOMETRY_SHADER);
	if(!result)
	{
		return false;
	}

	return true;
}

void DeferredShader::Shutdown()
{
	ShutdownShader();
}

bool DeferredShader::Render(int _indexCount)
{
	//Now render the prepared buffers with the shader
	//TODO: EVERY RENDER CALL I WILL HAVE TO RESET ALL SHADER CONSTANTS AND RESOURCES FOR EACH SHADER FILE
	RenderShader(_indexCount);

	return true;
}

void DeferredShader::Update(ChildMeshObject* _obj)
{
	//UpdateVertexShaderConstants(_obj->GetWorldMatrixF(), Game::camera->GetViewProjectionMatrixF());
}

bool DeferredShader::InitializeShader(int _vertexShaderIndex, int _pixelShaderIndex, int _geometryShaderIndex)
{
	vertexShaderIndex = _vertexShaderIndex;
	pixelShaderIndex = _pixelShaderIndex;
	geometryShaderIndex = _geometryShaderIndex;

	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout;

	polygonLayout.SemanticName = "POSITION";
	polygonLayout.SemanticIndex = 0;
	polygonLayout.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout.InputSlot = 0;
	polygonLayout.AlignedByteOffset = 0;
	polygonLayout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout.InstanceDataStepRate = 0;

	//Create the vertex input layout
	hr = D3D11Renderer::d3dDevice->CreateInputLayout(&polygonLayout, 1, 
		ShaderManager::vertexShaders[vertexShaderIndex].buffer->GetBufferPointer(),
		ShaderManager::vertexShaders[vertexShaderIndex].buffer->GetBufferSize(), &inputLayout);

	if(FAILED(hr))
	{
		return false;
	}

	return true;
}

void DeferredShader::ShutdownShader()
{
	if(constantBuffer)
	{
		constantBuffer->Release();
		constantBuffer = 0;
	}

	if(inputLayout)
	{
		inputLayout->Release();
		inputLayout = 0;
	}
}

void DeferredShader::SetShader()
{
	//Set the vertex input layout
	D3D11Renderer::d3dImmediateContext->IASetInputLayout(inputLayout);

	//Set the vertex and pixel shaders that will be used to render this triangle
	D3D11Renderer::d3dImmediateContext->VSSetShader(ShaderManager::vertexShaders[vertexShaderIndex].shader, NULL, 0);
	D3D11Renderer::d3dImmediateContext->GSSetShader(ShaderManager::geometryShaders[geometryShaderIndex].shader, NULL, 0);
	D3D11Renderer::d3dImmediateContext->PSSetShader(ShaderManager::pixelShaders[pixelShaderIndex].shader, NULL, 0);
}

bool DeferredShader::UpdateVertexShaderConstants(XMFLOAT4X4 _worldMatrix, XMFLOAT4X4 _viewProjMatrix)
{
	HRESULT hr;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType constantBufferData;
	unsigned int bufferNumber;

	XMMATRIX mWorld = XMMatrixTranspose(XMLoadFloat4x4(&_worldMatrix));
	XMMATRIX mViewProjection = XMMatrixTranspose(XMLoadFloat4x4(&_viewProjMatrix));

	XMStoreFloat4x4(&constantBufferData.world, mWorld);
	XMStoreFloat4x4(&constantBufferData.viewProjection, mViewProjection);

	hr = D3D11Renderer::d3dImmediateContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if(FAILED(hr))
	{
		return false;
	}

	memcpy(mappedResource.pData, &constantBufferData, sizeof(constantBufferData));	
	D3D11Renderer::d3dImmediateContext->Unmap(constantBuffer, 0);

	bufferNumber = 0;
	D3D11Renderer::d3dImmediateContext->VSSetConstantBuffers(bufferNumber, 1, &constantBuffer);

	return true;
}

bool DeferredShader::UpdatePixelShaderTextureConstants(ID3D11ShaderResourceView* _textureArray)
{
	if(_textureArray)
	{
		D3D11Renderer::d3dImmediateContext->PSSetShaderResources(0, 1, &_textureArray);
		return true;
	}
	return false;
}

void DeferredShader::RenderShader(int _indexCount)
{
	D3D11Renderer::d3dImmediateContext->OMSetRenderTargets(1, &D3D11Renderer::renderTargetView[0], D3D11Renderer::depthStencilView);
	D3D11Renderer::d3dImmediateContext->DrawIndexed(1, 0, 0);
}