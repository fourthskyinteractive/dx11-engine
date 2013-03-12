#include "ColorShader.h"
#include "../ShaderManager.h"
#include "../D3D11Renderer.h"


ColorShader::ColorShader()
{
	inputLayout = NULL;
	constantBuffer = NULL;
}

ColorShader::ColorShader(const ColorShader& _colorShader)
{

}

ColorShader::~ColorShader()
{

}

bool ColorShader::Initialize()
{
	bool result;

	result = InitializeShader(COLOR_VERTEX_SHADER, COLOR_PIXEL_SHADER);
	if(!result)
	{
		return false;
	}

	return true;
}

void ColorShader::Shutdown()
{
	ShutdownShader();
}

bool ColorShader::Render(int _indexCount, XMFLOAT4X4 _worldMatrix,
	XMFLOAT4X4 _viewProjMatrix)
{
	bool result;

	//Set the shader parameters that it will use for rendering
	result = UpdateShaderConstants(_worldMatrix, _viewProjMatrix);
	if(!result)
	{
		return false;
	}

	//Now render the prepared buffers with the shader
	RenderShader(_indexCount);

	return true;
}

bool ColorShader::InitializeShader(int _vertexShaderIndex, int _pixelShaderIndex)
{
	int numElements;
	HRESULT hr;
	vertexShaderIndex = 0;
	pixelShaderIndex = 0;
	//Setup the layout of the dcata that goes into the shader
	D3D11_INPUT_ELEMENT_DESC polygonLayout[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	//Get a count of the elements in the layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//Create the vertex input layout
	hr = D3D11Renderer::d3dDevice->CreateInputLayout(polygonLayout, 
		numElements, 
		ShaderManager::vertexShaders[vertexShaderIndex].buffer->GetBufferPointer(),
		ShaderManager::vertexShaders[vertexShaderIndex].buffer->GetBufferSize(), 
		&inputLayout);


	//Setup the description of the dynamic matrix constant buffer that is in the vertex shader
	D3D11_BUFFER_DESC matrixBufferDesc = {0};

	//CHECK THIS: if it doesnt work
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	hr = D3D11Renderer::d3dDevice->CreateBuffer(&matrixBufferDesc, nullptr, &constantBuffer);

	if(FAILED(hr))
	{
		return false;
	}

	D3D11Renderer::d3dImmediateContext->IASetInputLayout(inputLayout);

	D3D11Renderer::d3dImmediateContext->VSSetShader(ShaderManager::vertexShaders[vertexShaderIndex].shader, NULL, 0);
	D3D11Renderer::d3dImmediateContext->PSSetShader(ShaderManager::pixelShaders[pixelShaderIndex].shader, NULL, 0);

	D3D11Renderer::d3dImmediateContext->VSSetConstantBuffers(0, 1, &constantBuffer);

	return true;
}

void ColorShader::ShutdownShader()
{
	//Release the matrix constant buffer
	if(constantBuffer)
	{
		constantBuffer->Release();
		constantBuffer = 0;
	}

	//Release the layout
	if(inputLayout)
	{
		inputLayout->Release();
		inputLayout = 0;
	}
}

bool ColorShader::UpdateShaderConstants(XMFLOAT4X4 _worldMatrix, 
	XMFLOAT4X4 _viewProjMatrix)
{
	MatrixBufferType constantBufferData;
	XMMATRIX mWorld = XMMatrixTranspose(XMLoadFloat4x4(&_worldMatrix));
	XMMATRIX mViewProjection = XMMatrixTranspose(XMLoadFloat4x4(&_viewProjMatrix));

	XMStoreFloat4x4(&constantBufferData.world, mWorld);
	XMStoreFloat4x4(&constantBufferData.viewProjection, mViewProjection);

	D3D11_MAPPED_SUBRESOURCE edit;
	D3D11Renderer::d3dImmediateContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
	memcpy(edit.pData, &constantBufferData, sizeof(constantBufferData));
	D3D11Renderer::d3dImmediateContext->Unmap(constantBuffer, 0);

	return true;
}

void ColorShader::RenderShader(int _indexCount)
{
	D3D11Renderer::d3dImmediateContext->DrawIndexed(_indexCount, 0, 0);
}