#include "ColorShader.h"
#include "../ShaderManager.h"


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

bool ColorShader::Initialize(ID3D11Device* _device)
{
	bool result;

	result = InitializeShader(_device, COLOR_VERTEX_SHADER, COLOR_PIXEL_SHADER);
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

bool ColorShader::Render(ID3D11DeviceContext* _deviceContext, int _indexCount, XMFLOAT4X4 _worldMatrix,
	XMFLOAT4X4 _viewProjMatrix)
{
	bool result;

	//Set the shader parameters that it will use for rendering
	result = UpdateShaderConstants(_deviceContext, _worldMatrix, _viewProjMatrix);
	if(!result)
	{
		return false;
	}

	//Now render the prepared buffers with the shader
	RenderShader(_deviceContext, _indexCount);

	return true;
}

bool ColorShader::InitializeShader(ID3D11Device* _device, int _vertexShaderIndex, int _pixelShaderIndex)
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
	hr = _device->CreateInputLayout(polygonLayout, 
		numElements, 
		ShaderManager::vertexShaders[vertexShaderIndex].buffer->GetBufferPointer(),
		ShaderManager::vertexShaders[vertexShaderIndex].buffer->GetBufferSize(), 
		&inputLayout);


	//Setup the description of the dynamic matrix constant buffer that is in the vertex shader
	D3D11_BUFFER_DESC matrixBufferDesc = {0};

	//CHECK THIS: if it doesnt work
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = 0;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	hr = _device->CreateBuffer(&matrixBufferDesc, nullptr, &constantBuffer);

	if(FAILED(hr))
	{
		return false;
	}

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

bool ColorShader::UpdateShaderConstants(ID3D11DeviceContext* _deviceContext, XMFLOAT4X4 _worldMatrix, 
	XMFLOAT4X4 _viewProjMatrix)
{
	MatrixBufferType constantBufferData;
	XMMATRIX mWorld = XMMatrixTranspose(XMLoadFloat4x4(&_worldMatrix));
	XMMATRIX mViewProjection = XMMatrixTranspose(XMLoadFloat4x4(&_viewProjMatrix));

	XMStoreFloat4x4(&_worldMatrix, mWorld);
	XMStoreFloat4x4(&_viewProjMatrix, mViewProjection);

	constantBufferData.world = _worldMatrix;
	constantBufferData.viewProjection = _viewProjMatrix;

	_deviceContext->UpdateSubresource(
		constantBuffer,
		0,
		nullptr,
		&constantBufferData,
		0,
		0
		);

	return true;
}

void ColorShader::RenderShader(ID3D11DeviceContext* _deviceContext, int _indexCount)
{
	_deviceContext->IASetInputLayout(inputLayout);

	_deviceContext->VSSetShader(ShaderManager::vertexShaders[vertexShaderIndex].shader, NULL, 0);
	_deviceContext->PSSetShader(ShaderManager::pixelShaders[pixelShaderIndex].shader, NULL, 0);

	_deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

	_deviceContext->DrawIndexed(_indexCount, 0, 0);
}