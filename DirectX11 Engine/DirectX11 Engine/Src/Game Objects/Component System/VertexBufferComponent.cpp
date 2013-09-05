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

void VertexBufferComponent::AddVertexBufferComponent(VERTEX_BUFFER_COMPONENTS _component, void* _data, unsigned int _size)
{

	XMFLOAT3* test;
	test = (XMFLOAT3*)_data;
	vertexBufferComponentFlag |= (1 << _component);

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

	vertexBufferComponents.push_back(newVBComponent);
}

void VertexBufferComponent::RemoveVertexBufferComponent(VERTEX_BUFFER_COMPONENTS _component)
{
	if(vertexBufferComponentFlag & (1 << _component))
	{
		vertexBufferComponentFlag &= ~(1 << _component);

		for (vector<VBComponent*>::iterator iter = vertexBufferComponents.begin(); iter != vertexBufferComponents.end(); iter++)
		{
			if((*iter)->componentType == _component)
			{
				vertexBufferComponents.erase(iter);
				break;
			}
		}	
	}
}