#include "BaseObject.h"
#include "../Renderer/D3D11Renderer.h"
#include "../Game/Game.h"

BaseObject::BaseObject()
{
	baseComponent = new BaseComponent();
}

BaseObject::BaseObject(const BaseObject& _baseObject)
{

}

BaseObject::~BaseObject()
{
	delete baseComponent;
	baseComponent = NULL;
}

void BaseObject::AddBaseComponent(BASE_COMPONENTS _componentType)
{
	BaseComponent* componentToAdd;

	switch (_componentType)
	{
	case RENDER_COMPONENT:
		componentToAdd = new RenderComponent();
		baseComponent->AddBaseComponent(RENDER_COMPONENT, componentToAdd);
		break;
	default:
		break;
	}
}

void BaseObject::AddRenderComponent(RENDER_COMPONENTS _componentType)
{
	RenderComponent* componentToAdd;
	unsigned int renderComponentIndex;

	if(!(baseComponent->GetBaseComponentFlag() & (1 << _componentType)))
	{
		MessageBox(NULL, "There is not a Render Component in this object!", "Component System Error", 0);
		return;
	}

	for(unsigned int i = 0; i < baseComponent->GetBaseComponents().size(); ++i)
	{
		if(baseComponent->GetBaseComponents()[i].componentType == RENDER_COMPONENT)
		{
			renderComponentIndex = i;
			break;
		}
	}

	switch (_componentType)
	{
	case VERTEX_BUFFER_RENDER_COMPONENT:
		componentToAdd = new VertexBufferComponent();
		((RenderComponent*)baseComponent->GetBaseComponents()[renderComponentIndex].component)->AddRenderComponent(_componentType, componentToAdd);
		break;
	case INDEX_BUFFER_RENDER_COMPONENT:
		break;
	case CONSTANT_BUFFER_RENDER_COMPONENT:
		break;
	case TEXTURE_RENDER_COMPONENT:
		break;
	default:
		break;
	}
}

void BaseObject::AddVertexBufferComponent(VERTEX_BUFFER_COMPONENTS _componentType, void* _data, unsigned int _size)
{
	unsigned int renderComponentIndex;
	unsigned int vertexBufferComonentIndex;

	if(!(baseComponent->GetBaseComponentFlag() & (1 << RENDER_COMPONENT)))
	{
		MessageBox(NULL, "There is not a Render Component in this object!", "Component System Error", 0);
		return;
	}

	for(unsigned int i = 0; i < baseComponent->GetBaseComponents().size(); ++i)
	{
		if(baseComponent->GetBaseComponents()[i].componentType == RENDER_COMPONENT)
		{
			renderComponentIndex = i;
			break;
		}
	}

	if(!((RenderComponent*)baseComponent->GetBaseComponents()[renderComponentIndex].component)->GetRenderComponentFlag() & (1 << _componentType))
	{
		MessageBox(NULL, "There is not a Vertex Buffer Component in this object!", "Component System Error", 0);
		return;
	}

	unsigned int numRenderComponents = ((RenderComponent*)baseComponent->GetBaseComponents()[renderComponentIndex].component)->GetRenderComponents().size();
	for(unsigned int i = 0; i < numRenderComponents; ++i)
	{
		if(((RenderComponent*)baseComponent->GetBaseComponents()[renderComponentIndex].component)->GetRenderComponents()[i].componentType == _componentType)
		{
			vertexBufferComonentIndex = i;
			break;
		}
	}

	RenderComponent* tempRenderComponent = ((RenderComponent*)baseComponent->GetBaseComponents()[renderComponentIndex].component);
	VertexBufferComponent* tempVertexBufferComponent = ((VertexBufferComponent*)tempRenderComponent->GetRenderComponents()[vertexBufferComonentIndex].component);

	tempVertexBufferComponent->AddVertexBufferComponent(_componentType, _data, _size);
}