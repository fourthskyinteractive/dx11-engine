#include "ConstantBufferComponent.h"

ConstantBufferComponent::ConstantBufferComponent()
{
	memset(&constantBufferComponentFlag, 0, sizeof(char));
}

ConstantBufferComponent::ConstantBufferComponent(const ConstantBufferComponent& _vertexBufferComponent)
{

}

ConstantBufferComponent::~ConstantBufferComponent()
{
	for(unsigned int i = 0; i < constantBufferComponents.size(); ++i)
	{
		delete constantBufferComponents[i];
		constantBufferComponents[i] = NULL;
	}
}

void ConstantBufferComponent::AddConstantBufferComponent(CONSTANT_BUFFER_COMPONENTS _component, void* _data, unsigned int _size)
{
	constantBufferComponentFlag |= (1 << _component);

	ConstantComponent* newConstantComponent = new ConstantComponent;
	newConstantComponent->componentType = _component;
	newConstantComponent->size = _size;

	D3D11_BUFFER_DESC vBufferDesc;
	ZeroMemory(&vBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vBufferDesc.ByteWidth = _size;
	vBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA vData;
	ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
	vData.pSysMem = &_data;

	HRESULT hr = D3D11Renderer::d3dDevice->CreateBuffer(&vBufferDesc, &vData, &newConstantComponent->buffer);

	if(hr != S_OK)
	{
		MessageBox(NULL, "Vertex Buffer Creation Failed : VertexBufferComponent::AddVertexBufferComponent(VERTEX_BUFFER_COMPONENT _component, void* _data, unsigned int _size)", "Vertex Buffer Component Error", 0);
	}

	constantBufferComponents.push_back(newConstantComponent);
}

void ConstantBufferComponent::RemoveConstantBufferComponent(CONSTANT_BUFFER_COMPONENTS _component)
{
	if(constantBufferComponentFlag & (1 << _component))
	{
		constantBufferComponentFlag &= ~(1 << _component);

		for (vector<ConstantComponent*>::iterator iter = constantBufferComponents.begin(); iter != constantBufferComponents.end(); iter++)
		{
			if((*iter)->componentType == _component)
			{
				constantBufferComponents.erase(iter);
				break;
			}
		}	
	}
}