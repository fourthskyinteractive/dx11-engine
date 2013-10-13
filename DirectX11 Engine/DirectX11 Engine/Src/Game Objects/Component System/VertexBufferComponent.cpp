#include "VertexBufferComponent.h"

VertexBufferComponent::VertexBufferComponent()
{
	memset(&vertexBufferComponentFlag, 0, sizeof(char));
}

VertexBufferComponent::VertexBufferComponent(const VertexBufferComponent& _vertexBufferComponent)
{

}

VertexBufferComponent::~VertexBufferComponent()
{
	for(unsigned int i = 0; i < vertexBufferComponents.size(); ++i)
	{
		delete vertexBufferComponents[i];
		vertexBufferComponents[i] = NULL;
	}
}

void VertexBufferComponent::AddVertexBufferComponent(VERTEX_BUFFER_COMPONENTS _component, void* _data, unsigned int _stride, unsigned int _totalSize)
{
	vertexBufferComponentFlag |= (1 << _component);

	VertexComponent* newVBComponent = new VertexComponent;
	newVBComponent->componentType = _component;
	newVBComponent->totalSize = _totalSize;
	newVBComponent->stride = _stride;

	D3D11_BUFFER_DESC vBufferDesc;
	ZeroMemory(&vBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufferDesc.ByteWidth = _totalSize;
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vData;
	ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
	vData.pSysMem = _data;

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

		for (vector<VertexComponent*>::iterator iter = vertexBufferComponents.begin(); iter != vertexBufferComponents.end(); iter++)
		{
			if((*iter)->componentType == _component)
			{
				vertexBufferComponents.erase(iter);
				break;
			}
		}	
	}
}