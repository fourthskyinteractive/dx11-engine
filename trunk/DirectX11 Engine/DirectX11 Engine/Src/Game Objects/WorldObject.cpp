#include "WorldObject.h"
#include "../Game/Game.h"

#include "../../DeferredGeometryVertexShader.csh"
#include "../../DeferredGeometryPixelShader.csh"


WorldObject::WorldObject()
{

}

WorldObject::WorldObject(const WorldObject& _worldObject)
{

}

WorldObject::~WorldObject()
{

}

void WorldObject::UpdateShaderConstantBuffers()
{
	BaseObject::UpdateShaderConstantBuffers();

	HRESULT hr = 0;
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ConstantBufferComponent* cBufferComponent = GetConstantBufferComponent();

	CComPtr<ID3D11Buffer> cBuffer;
	int bufferTypeAndSlot = -1;

	unsigned int numComponents = cBufferComponent->GetNumberConstantBufferComponents();
	for(unsigned int i = 0; i < numComponents; ++i)
	{
		ZeroMemory(&mappedSubresource, sizeof(mappedSubresource));
		//Gets the Constant Buffer
		cBuffer = cBufferComponent->GetConstantBufferComponents()[i]->buffer;

		//Picks the bind slot according to what type of matrix it is
		bufferTypeAndSlot = cBufferComponent->GetConstantBufferComponents()[i]->componentType;

		//Map and copy over the data
		hr = D3D11Renderer::d3dImmediateContext->Map(cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);

		if(hr == S_OK)
		{
			if(bufferTypeAndSlot == WORLD_MATRIX_COMPONENT)
			{
				memcpy(mappedSubresource.pData, &worldMatrix, 64);
			}
			else if(bufferTypeAndSlot == VIEW_MATRIX_COMPONENT)
			{
				memcpy(mappedSubresource.pData, &Game::camera->GetViewMatrixF(), sizeof(XMFLOAT4X4));
			}
			else// if(bufferTypeAndSlot == PROJECTION_MATRIX_COMPONENT)
			{
				memcpy(mappedSubresource.pData, &Game::camera->GetProjectionMatrixF(), sizeof(XMFLOAT4X4));
			}
		}

		D3D11Renderer::d3dImmediateContext->Unmap(cBuffer, 0);

		D3D11Renderer::d3dImmediateContext->VSSetConstantBuffers(bufferTypeAndSlot, 1, &cBuffer.p);
	}
}

void WorldObject::BindRenderComponents()
{
	BaseObject::BindRenderComponents();

	DX11RenderDataMembers* renderDataMembers = GetRenderDataMembers();
	BuffersForBinding* buffersForBinding = GetBuffersForBinding();
	D3D11Renderer::d3dImmediateContext->OMSetRenderTargets(4, &D3D11Renderer::renderTargetView[1].p, D3D11Renderer::depthStencilView);
	D3D11Renderer::d3dImmediateContext->PSSetSamplers(0, 1, &renderDataMembers->samplerState.p);

// 	for(unsigned int i = 0; i < vertexBufferInformationForBinding.numBuffers; ++i)
// 	{
// 		D3D11Renderer::d3dImmediateContext->IASetVertexBuffers(i, 1, &vertexBufferInformationForBinding.buffers[i].p, &vertexBufferInformationForBinding.strides[i], &vertexBufferInformationForBinding.offsets[0]);
// 	}
	D3D11Renderer::d3dImmediateContext->IASetVertexBuffers(0, buffersForBinding->numBuffers, &buffersForBinding->buffers[0].p, &buffersForBinding->strides[0], &buffersForBinding->offsets[0]);
	D3D11Renderer::d3dImmediateContext->IASetIndexBuffer(GetIndexBufferComponent()->GetIndexBufferComponents()[0]->buffer, DXGI_FORMAT_R32_UINT, 0);
	D3D11Renderer::d3dImmediateContext->IASetInputLayout(renderDataMembers->inputLayout);
	D3D11Renderer::d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void WorldObject::FinalizeObject()
{
	if(GetRenderDataMembers()->vertexShader == NULL ||
		GetRenderDataMembers()->pixelShader == NULL)
	{
		MessageBox(NULL, "One or more of the shaders are not set! : WorldObject::FinalizeObject()", "Shader Set Error", 0);
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
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	ZeroMemory(&polygonLayout[0], sizeof(D3D11_INPUT_ELEMENT_DESC) * 3);	

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "NORMAL";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[1].InputSlot = 1;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "TEXCOORD";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[2].InputSlot = 2;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	CComPtr<ID3D11InputLayout> inputLayout;
	hr = D3D11Renderer::d3dDevice->CreateInputLayout(&polygonLayout[0], 3, 
		GetRenderDataMembers()->vertexBufferBytes,
		GetRenderDataMembers()->vertexBufferBiteWidth, &inputLayout);

	SetInputLayout(inputLayout);

	if(hr != S_OK)
	{
		MessageBox(NULL, "Input Layout Creation Error! : WorldObject::FinalizeObject()", "Input Layout Error", 0);
	}
}

void WorldObject::Render()
{
	if(!IsRenderable())
		return;

	BaseObject::Render();

	D3D11Renderer::d3dImmediateContext->VSSetShader(GetVertexShader(), NULL, 0);
	D3D11Renderer::d3dImmediateContext->GSSetShader(GetGeometryShader(), NULL, 0);
	D3D11Renderer::d3dImmediateContext->PSSetShader(GetPixelShader(), NULL, 0);
	
	//Update and set the Constant Buffers
	UpdateShaderConstantBuffers();

	//Set the vertex and index buffers
	BindRenderComponents();

	D3D11Renderer::d3dImmediateContext->DrawIndexed(GetNumberOfIndices(), 0, 0);
}

void WorldObject::Update(float _dt)
{
	if(!IsRenderable())
		return;

}

void WorldObject::Destroy()
{

	BaseObject::Destroy();
}