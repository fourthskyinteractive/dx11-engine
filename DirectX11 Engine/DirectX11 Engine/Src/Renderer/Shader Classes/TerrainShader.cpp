#include "TerrainShader.h"
#include "../ShaderManager.h"
#include "../D3D11Renderer.h"
#include "../../Game Objects/ChildMeshObject.h"
#include "../../Game/Game.h"
#include "../../Game Objects/Lights/LightManager.h"

TerrainShader::TerrainShader()
{
	inputLayout = NULL;
	sampleState = NULL;
	constantBuffer = NULL;
	SetShaderToUse(DIFFUSE_SHADER);
}

TerrainShader::TerrainShader(const TerrainShader& _ObjectShader)
{

}

TerrainShader::~TerrainShader()
{

}

bool TerrainShader::Initialize(VERTEX_SHADERS _vertexShader, PIXEL_SHADERS _pixelShader, GEOMETRY_SHADERS _geometryShader)
{
	bool result;

	vertexShaderIndex = _vertexShader;
	pixelShaderIndex = _pixelShader;
	geometryShaderIndex = _geometryShader;

	result = InitializeShader();

	if(!result)
	{
		return false;
	}

	return true;
}

void TerrainShader::Shutdown()
{
	ShutdownShader();
}

bool TerrainShader::Render(int _indexCount, ID3D11RenderTargetView* _renderTarget)
{
	//Now render the prepared buffers with the shader
	RenderShader(_indexCount, _renderTarget);

	return true;
}

void TerrainShader::Update(XMFLOAT4X4 _worldMat, ID3D11ShaderResourceView* _texture)
{
	if(_texture)
	{
		UpdatePixelShaderTextureConstants(_texture);
	}

	UpdateVertexShaderConstants(_worldMat, Game::camera->GetViewProjectionMatrixF());
	SetShader();
}

bool TerrainShader::InitializeShader()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout;
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;

	polygonLayout.SemanticName = "POSITION";
	polygonLayout.SemanticIndex = 0;
	polygonLayout.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout.InputSlot = 0;
	polygonLayout.AlignedByteOffset = 0;
	polygonLayout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout.InstanceDataStepRate = 0;

// 	polygonLayout[1].SemanticName = "TEXCOORD";
// 	polygonLayout[1].SemanticIndex = 0;
// 	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
// 	polygonLayout[1].InputSlot = 0;
// 	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
// 	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
// 	polygonLayout[1].InstanceDataStepRate = 0;
// 
// 	//One of the major changes to the shader initialization is here in the polygonLayout. 
// 	//We add a third element for the normal vector that will be used for lighting. 
// 	//The semantic name is NORMAL and the format is the regular DXGI_FORMAT_R32G32B32_FLOAT 
// 	//which handles 3 floats for the x, y, and z of the normal vector. The layout will now 
// 	//match the expected input to the HLSL vertex shader.
// 
// 	polygonLayout[2].SemanticName = "NORMAL";
// 	polygonLayout[2].SemanticIndex = 0;
// 	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
// 	polygonLayout[2].InputSlot = 0;
// 	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
// 	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
// 	polygonLayout[2].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout);

	//Create the vertex input layout
// 	hr = D3D11Renderer::d3dDevice->CreateInputLayout(&polygonLayout, numElements, 
// 		ShaderManager::vertexShaders[vertexShaderIndex].buffer->GetBufferPointer(),
// 		ShaderManager::vertexShaders[vertexShaderIndex].buffer->GetBufferSize(), &inputLayout);

// 	if(FAILED(hr))
// 	{
// 		return false;
// 	}

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

	return true;
}

void TerrainShader::ShutdownShader()
{
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

void TerrainShader::SetShader()
{
	//Set the vertex input layout
	D3D11Renderer::d3dImmediateContext->IASetInputLayout(inputLayout);

	//Set the vertex and pixel shaders that will be used to render this triangle
// 	D3D11Renderer::d3dImmediateContext->VSSetShader(ShaderManager::vertexShaders[vertexShaderIndex].shader, NULL, 0);
// 	D3D11Renderer::d3dImmediateContext->GSSetShader(NULL, NULL, 0);
// 	D3D11Renderer::d3dImmediateContext->PSSetShader(ShaderManager::pixelShaders[pixelShaderIndex].shader, NULL, 0);

	//Set the sampler state in the pixel shader
	D3D11Renderer::d3dImmediateContext->PSSetSamplers(0, 1, &sampleState);
}

bool TerrainShader::UpdateVertexShaderConstants(XMFLOAT4X4 _worldMatrix, XMFLOAT4X4 _viewProjMatrix)
{
	HRESULT hr;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType constantBufferData;
	unsigned int bufferNumber;

	XMMATRIX mWorld = XMLoadFloat4x4(&_worldMatrix);
	XMMATRIX mViewProjection = XMLoadFloat4x4(&_viewProjMatrix);

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

bool TerrainShader::UpdatePixelShaderTextureConstants(ID3D11ShaderResourceView* _textureArray)
{
	if(_textureArray)
	{
		D3D11Renderer::d3dImmediateContext->PSSetShaderResources(0, 1, &_textureArray);
		return true;
	}
	return false;
}

void TerrainShader::RenderShader(int _indexCount, ID3D11RenderTargetView* _renderTarget)
{
	D3D11Renderer::d3dImmediateContext->OMSetRenderTargets(1, &D3D11Renderer::renderTargetView[0].p, D3D11Renderer::depthStencilView);
	D3D11Renderer::d3dImmediateContext->DrawIndexed(_indexCount, 0, 0);
}