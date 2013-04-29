#include "LightShader.h"
#include "../ShaderManager.h"
#include "../D3D11Renderer.h"
#include "../../Game/Game.h"
#include "../../Game Objects/ChildMeshObject.h"

LightShader::LightShader()
{
	inputLayout = NULL;
	constantBuffer = NULL;
	SetBufferType(DEPTH_BUFFER);
}

LightShader::LightShader(const LightShader& _depthBuffer)
{

}

LightShader::~LightShader()
{

}

bool LightShader::Initialize()
{
	bool result;

	result = InitializeShader(SIMPLE_VERTEX_SHADER, SIMPLE_PIXEL_SHADER);
	if(!result)
	{
		return false;
	}

	return true;
}

void LightShader::Shutdown()
{
	ShutdownShader();
}

bool LightShader::Render(int _indexCount, ID3D11RenderTargetView* _renderTarget)
{
	//Now render the prepared buffers with the shader
	RenderShader(_indexCount, _renderTarget);
	return true;
}

void LightShader::Update(ChildMeshObject* _obj, ID3D11ShaderResourceView* _texture)
{
	if(_texture)
	{
		UpdatePixelShaderTextureConstants(_texture);
	}

	SetShader();
}

bool LightShader::InitializeShader(int _vertexShaderIndex, int _pixelShaderIndex)
{
	vertexShaderIndex = _vertexShaderIndex;
	pixelShaderIndex = _pixelShaderIndex;

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

void LightShader::ShutdownShader()
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

void LightShader::SetShader()
{
	//Set the vertex input layout
	D3D11Renderer::d3dImmediateContext->IASetInputLayout(inputLayout);

	//Set the vertex and pixel shaders that will be used to render this triangle
	D3D11Renderer::d3dImmediateContext->VSSetShader(ShaderManager::vertexShaders[vertexShaderIndex].shader, NULL, 0);
	D3D11Renderer::d3dImmediateContext->PSSetShader(ShaderManager::pixelShaders[pixelShaderIndex].shader, NULL, 0);
}

bool LightShader::UpdatePixelShaderTextureConstants(ID3D11ShaderResourceView* _textureArray)
{
	if(_textureArray)
	{
		D3D11Renderer::d3dImmediateContext->GenerateMips(_textureArray);
		D3D11Renderer::d3dImmediateContext->PSSetShaderResources(0, 1, &_textureArray);
		return true;
	}
	return false;
}

void LightShader::RenderShader(int _indexCount, ID3D11RenderTargetView* _renderTarget)
{
	D3D11Renderer::d3dImmediateContext->OMSetRenderTargets(1, &D3D11Renderer::renderTargetView[0], D3D11Renderer::depthStencilView);
	D3D11Renderer::d3dImmediateContext->DrawIndexed(_indexCount, 0,0);
}