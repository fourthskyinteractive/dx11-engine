#include "DynamicObject.h"
#include "../Renderer/D3D11Renderer.h"

DynamicObject::DynamicObject()
{
	ZeroMemory(&componentsBitFlag, sizeof(char));
}

DynamicObject::DynamicObject(const DynamicObject& _dynamicObject)
{

}

DynamicObject::~DynamicObject()
{

}


void DynamicObject::Render()
{

}

void DynamicObject::Update(float _dt)
{

}

void DynamicObject::AddComponent(RENDERCOMPONENTS _c)
{
	switch (_c)
	{
	case VERTEX_BUFFER_COMPONENT:
		{
			ZeroMemory(&vertexBufferComponent.childrenBitFlag, sizeof(char));
			componentsBitFlag |= (1 << VERTEX_BUFFER_COMPONENT);
			break;
		}
	case INDEX_BUFFER_COMPONENT:
		{
			componentsBitFlag |= (1 << INDEX_BUFFER_COMPONENT);
			break;
		}
	case CONSTANT_BUFFER_COMPONENT:
		{
			componentsBitFlag |= (1 << CONSTANT_BUFFER_COMPONENT);
			break;
		}
	case TEXTURE_COMPONENT:
		{
			componentsBitFlag |= (1 << TEXTURE_COMPONENT);
			break;
		}
	case ANIMATION_COMPONENT:
		{
			componentsBitFlag |= (1 << ANIMATION_COMPONENT);
			break;
		}
	default:
		{
			break;
		}
	}
}

void DynamicObject::RemoveComponent(RENDERCOMPONENTS _c)
{

}