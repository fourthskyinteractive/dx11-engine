#include "IndexBufferComponent.h"

IndexBufferComponent::IndexBufferComponent()
{
	memset(&indexBufferComponentFlag, 0, sizeof(char));
}

IndexBufferComponent::IndexBufferComponent(const IndexBufferComponent& _vertexBufferComponent)
{

}

IndexBufferComponent::~IndexBufferComponent()
{
	for(unsigned int i = 0; i < indexBufferComponents.size(); ++i)
	{
		delete indexBufferComponents[i];
		indexBufferComponents[i] = NULL;
	}
}

void IndexBufferComponent::AddIndexBufferComponent(INDEX_BUFFER_COMPONENTS _component, void* _data, unsigned int _size)
{
	unsigned int* test;
	test = (unsigned int*)_data;
	indexBufferComponentFlag |= (1 << _component);

	IndexComponent* newIndexComponent = new IndexComponent;
	newIndexComponent->componentType = _component;
	newIndexComponent->size = _size;

	D3D11_BUFFER_DESC vBufferDesc;
	ZeroMemory(&vBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufferDesc.ByteWidth = _size;
	vBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vData;
	ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
	vData.pSysMem = &_data;

	HRESULT hr = D3D11Renderer::d3dDevice->CreateBuffer(&vBufferDesc, &vData, &newIndexComponent->buffer);

	if(hr != S_OK)
	{
		MessageBox(NULL, "Index Buffer Creation Failed : IndexBufferComponent::AddIndexBufferComponent(INDEX_BUFFER_COMPONENTS _component, void* _data, unsigned int _size)", "Index Buffer Component Error", 0);
	}

	indexBufferComponents.push_back(newIndexComponent);
}

void IndexBufferComponent::RemoveIndexBufferComponent(INDEX_BUFFER_COMPONENTS _component)
{
	if(indexBufferComponentFlag & (1 << _component))
	{
		indexBufferComponentFlag &= ~(1 << _component);

		for (vector<IndexComponent*>::iterator iter = indexBufferComponents.begin(); iter != indexBufferComponents.end(); iter++)
		{
			if((*iter)->componentType == _component)
			{
				indexBufferComponents.erase(iter);
				break;
			}
		}	
	}
}