#include "ScreenSpaceObject.h"
#include "../Game/Game.h"

ScreenSpaceObject::ScreenSpaceObject()
{

}

ScreenSpaceObject::ScreenSpaceObject(const ScreenSpaceObject& _screenSpaceObject)
{

}

ScreenSpaceObject::~ScreenSpaceObject()
{

}

void ScreenSpaceObject::UpdateShaderConstantBuffers()
{
	BaseObject::UpdateShaderConstantBuffers();

	HRESULT hr = 0;
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ConstantBufferComponent* cBufferComponent = GetConstantBufferComponent();

	CComPtr<ID3D11Buffer> cBuffer;

	unsigned int numComponents = cBufferComponent->GetNumberConstantBufferComponents();
	for(unsigned int i = 0; i < numComponents; ++i)
	{
		ZeroMemory(&mappedSubresource, sizeof(mappedSubresource));
		//Gets the Constant Buffer
		cBuffer = cBufferComponent->GetConstantBufferComponents()[i]->buffer;
		void* memoryPointer = cBufferComponent->GetConstantBufferComponents()[i]->memoryAddress;
		if(memoryPointer)
		{
			//Map and copy over the data
			hr = D3D11Renderer::d3dImmediateContext->Map(cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);

			if(hr == S_OK)
			{
				memcpy(mappedSubresource.pData, cBufferComponent->GetConstantBufferComponents()[i]->memoryAddress, cBufferComponent->GetConstantBufferComponents()[i]->size);

				//if(bufferTypeAndSlot == WORLD_MATRIX_COMPONENT)
				//{
				//	memcpy(mappedSubresource.pData, &GetWorldMatrixF(), cBufferComponent->GetConstantBufferComponents()[i]->size);
				//}
				//else if(bufferTypeAndSlot == VIEW_MATRIX_COMPONENT)
				//{
				//	memcpy(mappedSubresource.pData, cBufferComponent->GetConstantBufferComponents()[i]->updateFunctionPointer(), cBufferComponent->GetConstantBufferComponents()[i]->size);
				//}
				//else// if(bufferTypeAndSlot == PROJECTION_MATRIX_COMPONENT)
				//{
				//	memcpy(mappedSubresource.pData, cBufferComponent->GetConstantBufferComponents()[i]->updateFunctionPointer(), cBufferComponent->GetConstantBufferComponents()[i]->size);
				//}
			}

			D3D11Renderer::d3dImmediateContext->Unmap(cBuffer, 0);
		}

		unsigned int bufferSlot = cBufferComponent->GetConstantBufferComponents()[i]->bufferSlot;
		switch (cBufferComponent->GetConstantBufferComponents()[i]->associatedShader)
		{
		case VERTEX_SHADER:
			D3D11Renderer::d3dImmediateContext->VSSetConstantBuffers(bufferSlot, 1, &cBuffer.p);
			break;
		case GEOMETRY_SHADER:
			D3D11Renderer::d3dImmediateContext->GSSetConstantBuffers(bufferSlot, 1, &cBuffer.p);
			break;
		case PIXEL_SHADER:
			D3D11Renderer::d3dImmediateContext->PSSetConstantBuffers(bufferSlot, 1, &cBuffer.p);
			break;
		case COMPUTE_SHADER:
			D3D11Renderer::d3dImmediateContext->CSSetConstantBuffers(bufferSlot, 1, &cBuffer.p);
			break;
		default:
			break;
		}
	}
}

void ScreenSpaceObject::BindRenderComponents()
{
	BaseObject::BindRenderComponents();

	DX11RenderDataMembers* renderDataMembers = GetRenderDataMembers();
	BuffersForBinding* buffersForBinding = GetBuffersForBinding();

	//D3D11Renderer::d3dImmediateContext->OMSetRenderTargets(1, &D3D11Renderer::renderTargetView[0].p, D3D11Renderer::depthStencilView);
	//TODO:
	//FIGURE OUT A WAY TO TELL THIS OBJECT WHAT SHADER RESOURCES TO BIND!

	D3D11Renderer::d3dImmediateContext->PSSetSamplers(0, 1, &renderDataMembers->samplerState.p);
	D3D11Renderer::d3dImmediateContext->IASetVertexBuffers(0, buffersForBinding->numBuffers, &buffersForBinding->buffers[0].p, &buffersForBinding->strides[0], &buffersForBinding->offsets[0]);
	D3D11Renderer::d3dImmediateContext->IASetInputLayout(renderDataMembers->inputLayout);
	D3D11Renderer::d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
}

void ScreenSpaceObject::FinalizeObject()
{
	if(GetRenderDataMembers()->vertexShader == NULL ||
		GetRenderDataMembers()->pixelShader == NULL || 
		GetRenderDataMembers()->geometryShader == NULL ||
		GetRenderDataMembers()->computeShader == NULL)
	{
		MessageBox(NULL, "One or more of the shaders are not set! : ScreenSpaceObject::FinalizeObject()", "Shader Set Error", 0);
		return;
	}

	HRESULT hr;
	BaseObject::FinalizeObject();

	//Set up all of the buffers that this object will use.
	//1. Vertex Buffer
	//2. Index Buffer
	//3. Constant Buffers	
	SetRenderable(true);

	//Put the Vertex Buffers in one place
	VertexBufferComponent* vBufferComponent = GetVertexBufferComponent();
	BuffersForBinding* buffersForBinding = GetBuffersForBinding();
	unsigned int numComponents = vBufferComponent->GetNumberVertexBufferComponents();

	unsigned int totalOffset = 0;
	for(unsigned int i = 0; i < numComponents; ++i)
	{
		buffersForBinding->buffers.push_back(vBufferComponent->GetVertexBufferComponents()[i]->buffer);
		buffersForBinding->strides.push_back(vBufferComponent->GetVertexBufferComponents()[i]->stride);
		buffersForBinding->offsets.push_back(0);
		buffersForBinding->numBuffers += 1;

		totalOffset += vBufferComponent->GetVertexBufferComponents()[i]->totalSize;
	}

	//Setup the input Layout
	D3D11_INPUT_ELEMENT_DESC polygonLayout;
	ZeroMemory(&polygonLayout, sizeof(D3D11_INPUT_ELEMENT_DESC));	

	polygonLayout.SemanticName = "POSITION";
	polygonLayout.SemanticIndex = 0;
	polygonLayout.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout.InputSlot = 0;
	polygonLayout.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout.InstanceDataStepRate = 0;

	CComPtr<ID3D11InputLayout> inputLayout;
	hr = D3D11Renderer::d3dDevice->CreateInputLayout(&polygonLayout, 1, 
		GetRenderDataMembers()->vertexBufferBytes,
		GetRenderDataMembers()->vertexBufferBiteWidth, &inputLayout);

	if(hr != S_OK)
	{
		MessageBox(NULL, "Input Layout Creation Error! : ScreenSpaceObject::FinalizeObject()", "Input Layout Error", 0);
		return;
	}

	SetInputLayout(inputLayout);	
}

void ScreenSpaceObject::Render()
{
	if(!IsRenderable())
		return;

	BaseObject::Render();

	D3D11Renderer::d3dImmediateContext->VSSetShader(GetVertexShader(), NULL, 0);
	D3D11Renderer::d3dImmediateContext->GSSetShader(GetGeometryShader(), NULL, 0);
	D3D11Renderer::d3dImmediateContext->PSSetShader(GetPixelShader(), NULL, 0);
	D3D11Renderer::d3dImmediateContext->CSSetShader(GetComputeShader(), NULL, 0);

	//Update and set the Constant Buffers
	UpdateShaderConstantBuffers();

	//Set the vertex and index buffers
	BindRenderComponents();

	D3D11Renderer::d3dImmediateContext->Dispatch(64, 64, 1);

	//D3D11Renderer::d3dImmediateContext->Draw(1, 0);
}

void ScreenSpaceObject::Update(float _dt)
{
	if(!IsRenderable())
		return;
}

void ScreenSpaceObject::Destroy()
{
	BaseObject::Destroy();
}