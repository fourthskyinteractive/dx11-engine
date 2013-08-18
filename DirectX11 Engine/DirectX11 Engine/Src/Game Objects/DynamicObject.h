#ifndef DYNAMICOBJECT_H
#define DYNAMICOBJECT_H

#include "../Game/Definitions.h"
#include <vector>
using namespace std;
#include <atlbase.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <dxgi.h>
using namespace DirectX;

struct VertexBufferComponentChild
{
	string semantic;
	unsigned int size;
	VERTEXBUFFERCOMPONENTS componentType;
	CComPtr<ID3D11Buffer> buffer;
};

struct VertexBufferComponentParent
{
	char childrenBitFlag;
	vector<VertexBufferComponentChild> components;
};

struct IndextBufferComponentChild
{
	int size;
	CComPtr<ID3D11Buffer> buffer;
};

struct IndexBufferComponentParent
{
	vector<IndextBufferComponentChild> components;
};

class DynamicObject
{
public:
	DynamicObject();
	DynamicObject(const DynamicObject& _dynamicObject);
	~DynamicObject();

private:
	char componentsBitFlag;

	VertexBufferComponentParent vertexBufferComponent;
	IndexBufferComponentParent indexBufferComponent;
	vector<CComPtr<ID3D11Buffer>> constantBufferComponents;
	vector<CComPtr<ID3D11ShaderResourceView>> textureComponents;

public:
	void Render();
	void Update(float _dt);

	void AddComponent(RENDERCOMPONENTS _c);
	void RemoveComponent(RENDERCOMPONENTS _c);

	template<typename SpecificComponent, typename ActualComponent>
	void AddToComponent(RENDERCOMPONENTS _c, SpecificComponent _componentToAdd, ActualComponent _component, unsigned int _size = 0, string _semantic = "");

	template<typename SpecificComponent>
	void RemoveFromComponent(RENDERCOMPONENTS _c, SpecificComponent _componentToRemove);
};

template<typename ComponentType, typename ActualComponent>
void DynamicObject::AddToComponent(RENDERCOMPONENTS _c, ComponentType _componentToAdd, ActualComponent _component, unsigned int _size, string _semantic)
{
	switch (_c)
	{
	case VERTEX_BUFFER_COMPONENT:
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
			desc.ByteWidth = _size;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.Usage = D3D11_USAGE_DEFAULT;

			D3D11_SUBRESOURCE_DATA initData;
			ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
			initData.pSysMem = _component;

			CComPtr<ID3D11Buffer> vertBuffer;
			D3D11Renderer::d3dDevice->CreateBuffer(&desc, &initData, &vertBuffer);

			VertexBufferComponentChild vertComponent;
			vertComponent.buffer = vertBuffer;
			vertComponent.componentType = _componentToAdd;
			vertComponent.semantic = _semantic;
			vertComponent.size = _size;
			vertexBufferComponent.components.push_back(vertComponent);

			vertexBufferComponent.childrenBitFlag |= (1 << _componentToAdd);

			break;
		}
	case INDEX_BUFFER_COMPONENT:
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
			desc.ByteWidth = _size;
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.Usage = D3D11_USAGE_DEFAULT;

			D3D11_SUBRESOURCE_DATA initData;
			ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
			initData.pSysMem = _component;

			CComPtr<ID3D11Buffer> indexBuffer;
			D3D11Renderer::d3dDevice->CreateBuffer(&desc, &initData, &indexBuffer);

			IndextBufferComponentChild indexComponent;
			indexComponent.size = _size;
			indexComponent.buffer = indexBuffer;

			indexBufferComponent.components.push_back(indexComponent);
			
			break;
		}
	case CONSTANT_BUFFER_COMPONENT:
		{

			break;
		}
	case TEXTURE_COMPONENT:
		{

			break;
		}
	case ANIMATION_COMPONENT:
		{

			break;
		}
	default:
		{

			break;
		}
	}
}

template<typename ComponentType>
void DynamicObject::RemoveFromComponent(RENDERCOMPONENTS _c, ComponentType _componentToRemove)
{
	//POSITION_VERTEX_COMPONENT, NORMAL_VERTEX_COMPONENT, UV_VERTEX_COMPONENT
	switch (_c)
	{
	case VERTEX_BUFFER_COMPONENT:
		{
			vector<VertexBufferComponentChild>::iterator iter;
			for(iter = vertexBufferComponent.components.begin(); iter != vertexBufferComponent.components.end(); ++iter)
			{
				if(iter->componentType == _componentToRemove)
				{
					vertexBufferComponent.components.erase(iter);
				}
			}

			break;
		}
	case INDEX_BUFFER_COMPONENT:
		{

			break;
		}
	case CONSTANT_BUFFER_COMPONENT:
		{

			break;
		}
	case TEXTURE_COMPONENT:
		{

			break;
		}
	case ANIMATION_COMPONENT:
		{

			break;
		}
	default:
		{

			break;
		}
	}
}

#endif
