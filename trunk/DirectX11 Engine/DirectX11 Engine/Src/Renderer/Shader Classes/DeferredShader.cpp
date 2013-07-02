#include "DeferredShader.h"
#include "../ShaderManager.h"
#include "../D3D11Renderer.h"
#include "../../Game/Game.h"
#include "../../Game Objects/ChildMeshObject.h"

DeferredShader::DeferredShader()
{
	inputLayout = NULL;
	constantBuffer = NULL;
	pixelConstantBuffer = NULL;
	SetShaderToUse(DEPTH_SHADER);
}

DeferredShader::DeferredShader(const DeferredShader& _depthBuffer)
{

}

DeferredShader::~DeferredShader()
{

}

bool DeferredShader::Initialize(VERTEX_SHADERS _vertexShader, PIXEL_SHADERS _pixelShader, GEOMETRY_SHADERS _geometryShader)
{
	bool result;

	result = InitializeShader(_vertexShader, _pixelShader, _geometryShader);
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
	D3D11Renderer::TurnZBufferOff();
	RenderShader(_indexCount);
	D3D11Renderer::TurnZBufferOn();
	return true;
}

void DeferredShader::UpdatePerFrame(ID3D11RenderTargetView* _renderTarget, ID3D11ShaderResourceView* _texture[])
{
	if(_texture)
	{
		UpdatePixelShaderTextureConstants(_texture);
	}

	SetShader();

	D3D11Renderer::d3dImmediateContext->OMSetRenderTargets(1, &_renderTarget, D3D11Renderer::depthStencilView);
}

void DeferredShader::UpdatePerLight(LIGHT_TYPE _lightType, int _lightIndex)
{
	UpdatePixelShaderConstants(_lightType, _lightIndex);
}

bool DeferredShader::InitializeShader(int _vertexShaderIndex, int _pixelShaderIndex, int _geometryShaderIndex)
{
	vertexShaderIndex = _vertexShaderIndex;
	pixelShaderIndex = _pixelShaderIndex;
	geometryShaderIndex = _geometryShaderIndex;

	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;

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

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(CombinePixelShaderBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = D3D11Renderer::d3dDevice->CreateBuffer(&matrixBufferDesc, NULL, &pixelConstantBuffer);
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
	//D3D11Renderer::d3dImmediateContext->PSSetSamplers(0, 1, &sampleState);
}

bool DeferredShader::UpdatePixelShaderTextureConstants(ID3D11ShaderResourceView* _textureArray[])
{
	if(!_textureArray)
		return false;

	D3D11Renderer::d3dImmediateContext->PSSetShaderResources(0, 4, &_textureArray[0]);

	return true;
}

bool DeferredShader::UpdatePixelShaderConstants(LIGHT_TYPE _lightType, int lightIndex)
{
	HRESULT hr;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CombinePixelShaderBufferType constantBufferData;

	unsigned int bufferNumber;

	constantBufferData.lightType.x = 0.0f;
	constantBufferData.lightType.y = 0.0f;
	constantBufferData.lightType.z = 0.0f;
	constantBufferData.lightType.w = 0.0f;



	if(_lightType == POINT_LIGHT)
	{
		constantBufferData.lightType.x = 1.0f;

		constantBufferData.lightColor = XMFLOAT3(	LightManager::GetPointLight(lightIndex)->GetLightColorF().x, 
													LightManager::GetPointLight(lightIndex)->GetLightColorF().y, 
													LightManager::GetPointLight(lightIndex)->GetLightColorF().z);

		constantBufferData.lightDirection = XMFLOAT3(0.0f, 0.0f, 0.0f);

		constantBufferData.lightPos = LightManager::GetPointLight(lightIndex)->GetLightPositionF();

		constantBufferData.lightRange = XMFLOAT4(	LightManager::GetPointLight(lightIndex)->GetLightRadius(),
													LightManager::GetPointLight(lightIndex)->GetLightRadius(),
													LightManager::GetPointLight(lightIndex)->GetLightRadius(),
													LightManager::GetPointLight(lightIndex)->GetLightRadius());
		
		constantBufferData.spotlightAngles = XMFLOAT2(0.0f, 0.0f);
	}
	else if(_lightType == SPOT_LIGHT)
	{
		constantBufferData.lightType.y = 1.0f;
	}
	else if(_lightType == DIRCTIONAL_LIGHT)
	{
		constantBufferData.lightType.z = 1.0f;

		constantBufferData.lightColor = XMFLOAT3(	LightManager::GetDirectionalLight(lightIndex)->GetLightColorF().x, 
													LightManager::GetDirectionalLight(lightIndex)->GetLightColorF().y, 
													LightManager::GetDirectionalLight(lightIndex)->GetLightColorF().z);

		constantBufferData.lightDirection = LightManager::GetDirectionalLight(lightIndex)->GetLightDirectionF();

		constantBufferData.lightPos = XMFLOAT3(0.0f, 0.0f, 0.0f);

		constantBufferData.lightRange = XMFLOAT4(0,	0, 0, 0);

		constantBufferData.spotlightAngles = XMFLOAT2(0.0f, 0.0f);
	}
	else if(_lightType == AMBIENT_LIGHT)
	{
		constantBufferData.lightType.w = 1.0f;
		constantBufferData.lightColor = XMFLOAT3(	LightManager::GetAmbientLight()->GetLightColorF().x, 
													LightManager::GetAmbientLight()->GetLightColorF().y, 
													LightManager::GetAmbientLight()->GetLightColorF().z);
	}
	else
	{
		return false;
	}

	constantBufferData.cameraPos = Game::camera->GetPosition();

	hr = D3D11Renderer::d3dImmediateContext->Map(pixelConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if(FAILED(hr))
	{
		return false;
	}

	memcpy(mappedResource.pData, &constantBufferData, sizeof(constantBufferData));	
	D3D11Renderer::d3dImmediateContext->Unmap(pixelConstantBuffer, 0);

	bufferNumber = 0;
	D3D11Renderer::d3dImmediateContext->PSSetConstantBuffers(bufferNumber, 1, &pixelConstantBuffer);

	return true;
}

void DeferredShader::RenderShader(int _indexCount)
{
	D3D11Renderer::d3dImmediateContext->Draw(1,0);
}