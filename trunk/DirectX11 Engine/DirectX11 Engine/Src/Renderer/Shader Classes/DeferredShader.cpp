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

bool DeferredShader::Render(int _indexCount, ID3D11RenderTargetView* _renderTarget)
{
	//Now render the prepared buffers with the shader
	//TODO: EVERY RENDER CALL I WILL HAVE TO RESET ALL SHADER CONSTANTS AND RESOURCES FOR EACH SHADER FILE
	D3D11Renderer::TurnZBufferOff();
	RenderShader(_indexCount, _renderTarget);
	D3D11Renderer::TurnZBufferOn();
	return true;
}

void DeferredShader::Update(ChildMeshObject* _obj, ID3D11ShaderResourceView* _texture)
{
	if(_texture)
	{
		UpdatePixelShaderTextureConstants(_texture);
	}

	SetShader();
}

bool DeferredShader::InitializeShader(int _vertexShaderIndex, int _pixelShaderIndex, int _geometryShaderIndex)
{
	vertexShaderIndex = _vertexShaderIndex;
	pixelShaderIndex = _pixelShaderIndex;
	geometryShaderIndex = _geometryShaderIndex;

	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout;
	D3D11_SAMPLER_DESC samplerDesc;

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

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	hr = D3D11Renderer::d3dDevice->CreateSamplerState(&samplerDesc, &sampleState);
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
	D3D11Renderer::d3dImmediateContext->PSSetSamplers(0, 1, &sampleState);
}

bool DeferredShader::UpdatePixelShaderTextureConstants(ID3D11ShaderResourceView* _textureArray)
{
	if(_textureArray)
	{
		D3D11Renderer::d3dImmediateContext->GenerateMips(_textureArray);
		D3D11Renderer::d3dImmediateContext->PSSetShaderResources(0, 1, &_textureArray);
		return true;
	}
	return false;
}

void DeferredShader::RenderShader(int _indexCount, ID3D11RenderTargetView* _renderTarget)
{
	D3D11Renderer::d3dImmediateContext->OMSetRenderTargets(1, &_renderTarget, D3D11Renderer::depthStencilView);
	D3D11Renderer::d3dImmediateContext->Draw(1,0);
}