#include "VertexBufferComponent.h"

VertexBufferComponent::VertexBufferComponent()
{

}

VertexBufferComponent::VertexBufferComponent(const VertexBufferComponent& _vertexBufferComponent)
{

}

VertexBufferComponent::~VertexBufferComponent()
{

}

void VertexBufferComponent::AddVertexBufferComponent(VERTEX_BUFFER_COMPONENT _component, void* _data, unsigned int _size)
{
	VBComponent* newVBComponent = new VBComponent;
	newVBComponent->componentType = _component;
	newVBComponent->size = _size;

	D3D11_BUFFER_DESC vBufferDesc;
	ZeroMemory(&vBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufferDesc.ByteWidth = _size;
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vData;
	ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
	vData.pSysMem = &_data;

	HRESULT hr = D3D11Renderer::d3dDevice->CreateBuffer(&vBufferDesc, &vData, &newVBComponent->buffer);

	if(hr != S_OK)
	{
		MessageBox(NULL, "Vertex Buffer Creation Failed : VertexBufferComponent::AddVertexBufferComponent(VERTEX_BUFFER_COMPONENT _component, void* _data, unsigned int _size)", "Vertex Buffer Component Error", 0);
	}
}

void VertexBufferComponent::RemoveVertexBufferComponent(VERTEX_BUFFER_COMPONENT _component)
{
	for (vector<VBComponent*>::iterator iter = vbComponents.begin(); iter != vbComponents.end(); iter++)
	{
		if((*iter)->componentType == _component)
		{
			vbComponents.erase(iter);
			break;
		}
	}	
}