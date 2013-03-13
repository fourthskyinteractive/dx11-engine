#include "LightShader.h"
#include "../ShaderManager.h"
#include "../D3D11Renderer.h"

LightShader::LightShader()
{
	inputLayout = NULL;
	sampleState = NULL;
	constantBuffer = NULL;
	lightBuffer = NULL;
}

LightShader::LightShader(const LightShader& _lightShader)
{

}

LightShader::~LightShader()
{

}

bool LightShader::Initialize()
{
	bool result;

	result = InitializeShader(TEXTURE_VERTEX_SHADER, TEXTURE_PIXEL_SHADER);
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

bool LightShader::Render(XMFLOAT4X4 _world, XMFLOAT4X4 _viewProjection, ID3D11ShaderResourceView* _texture,
						 XMFLOAT3 _lightDirection, XMFLOAT4 _diffuseColor, int _indexCount)
{
	bool result;

	//Set the shader parameters that it will use for rendering
	result = UpdateShaderConstants(_world, _viewProjection, _texture, _lightDirection, _diffuseColor);
	if(!result)
	{
		return false;
	}

	//Now render the prepared buffers with the shader
	RenderShader(_indexCount);

	return true;
}

bool LightShader::InitializeShader(int _vertexShaderIndex, int _pixelShaderIndex)
{
	vertexShaderIndex = _vertexShaderIndex;
	pixelShaderIndex = _pixelShaderIndex;
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//One of the major changes to the shader initialization is here in the polygonLayout. 
	//We add a third element for the normal vector that will be used for lighting. 
	//The semantic name is NORMAL and the format is the regular DXGI_FORMAT_R32G32B32_FLOAT 
	//which handles 3 floats for the x, y, and z of the normal vector. The layout will now 
	//match the expected input to the HLSL vertex shader.

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//Create the vertex input layout
	hr = D3D11Renderer::d3dDevice->CreateInputLayout(polygonLayout, numElements, 
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
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = D3D11Renderer::d3dDevice->CreateBuffer(&matrixBufferDesc, NULL, &constantBuffer);
	if(FAILED(hr))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = D3D11Renderer::d3dDevice->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
	if(FAILED(hr))
	{
		return false;
	}

	//Set the vertex input layout
	D3D11Renderer::d3dImmediateContext->IASetInputLayout(inputLayout);

	//Set the vertex and pixel shaders that will be used to render this triangle
	D3D11Renderer::d3dImmediateContext->VSSetShader(ShaderManager::vertexShaders[vertexShaderIndex].shader, NULL, 0);
	D3D11Renderer::d3dImmediateContext->PSSetShader(ShaderManager::pixelShaders[pixelShaderIndex].shader, NULL, 0);

	//Set the sampler state in the pixel shader
	D3D11Renderer::d3dImmediateContext->PSSetSamplers(0, 1, &sampleState);

	return true;
}

void LightShader::ShutdownShader()
{
	if(lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	if(constantBuffer)
	{
		constantBuffer->Release();
		constantBuffer = 0;
	}

	if(sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	if(inputLayout)
	{
		inputLayout->Release();
		inputLayout = 0;
	}
}

bool LightShader::UpdateShaderConstants(XMFLOAT4X4 _worldMatrix, XMFLOAT4X4 _viewProjMatrix, ID3D11ShaderResourceView* _texture, XMFLOAT3 _lightDirection, XMFLOAT4 _diffuseColor)
{
	HRESULT hr;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType constantBufferData;
	LightBufferType lightBufferData;
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

	D3D11Renderer::d3dImmediateContext->PSSetShaderResources(0, 1, &_texture);

	lightBufferData.diffuseColor = _diffuseColor;
	lightBufferData.lightDirection = _lightDirection;
	lightBufferData.padding = 0.0f;

	hr = D3D11Renderer::d3dImmediateContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(hr))
	{
		return false;
	}
	memcpy(mappedResource.pData, &lightBufferData, sizeof(lightBufferData));
	D3D11Renderer::d3dImmediateContext->Unmap(lightBuffer, 0);

	bufferNumber = 0;

	D3D11Renderer::d3dImmediateContext->PSSetConstantBuffers(bufferNumber, 1, &lightBuffer);

	return true;
}

void LightShader::RenderShader(int _indexCount)
{
	D3D11Renderer::d3dImmediateContext->DrawIndexed(_indexCount, 0, 0);
}