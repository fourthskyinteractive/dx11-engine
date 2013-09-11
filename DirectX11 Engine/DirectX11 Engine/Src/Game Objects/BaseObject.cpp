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
	RenderComponent* renderComponent;

	if(!(baseComponent->GetBaseComponentFlag() & (1 << RENDER_COMPONENT)))
	{
		MessageBox(NULL, "There is not a Render Component in this object!", "Component System Error", 0);
		return;
	}

	renderComponent = GetRenderComponent();

	switch (_componentType)
	{
	case VERTEX_BUFFER_RENDER_COMPONENT:
		componentToAdd = new VertexBufferComponent();
		renderComponent->AddRenderComponent(_componentType, componentToAdd);
		break;
	case INDEX_BUFFER_RENDER_COMPONENT:
		componentToAdd = new IndexBufferComponent();
		renderComponent->AddRenderComponent(_componentType, componentToAdd);
		break;
	case CONSTANT_BUFFER_RENDER_COMPONENT:
		componentToAdd = new ConstantBufferComponent();
		renderComponent->AddRenderComponent(_componentType, componentToAdd);
		break;
	case TEXTURE_RENDER_COMPONENT:
		break;
	default:
		break;
	}
}

void BaseObject::AddVertexBufferComponent(VERTEX_BUFFER_COMPONENTS _componentType, void* _data, unsigned int _size)
{
	unsigned int vertexBufferComponentIndex;

	RenderComponent* renderComponent = GetRenderComponent();

	if(renderComponent == NULL)
		return;

	if(!(renderComponent->GetRenderComponentFlag() & (1 << _componentType)))
	{
		MessageBox(NULL, "There is not a Vertex Buffer Component in this object!", "Component System Error", 0);
		return;
	}

	unsigned int numRenderComponents = renderComponent->GetRenderComponents().size();
	for(unsigned int i = 0; i < numRenderComponents; ++i)
	{
		if(renderComponent->GetRenderComponents()[i].componentType == VERTEX_BUFFER_RENDER_COMPONENT)
		{
			vertexBufferComponentIndex = i;
			break;
		}
	}

	VertexBufferComponent* vertexBufferComponent = ((VertexBufferComponent*)renderComponent->GetRenderComponents()[vertexBufferComponentIndex].component);

	vertexBufferComponent->AddVertexBufferComponent(_componentType, _data, _size);
}

void BaseObject::AddIndexBufferComponent(INDEX_BUFFER_COMPONENTS _componentType, void* _data, unsigned int _size)
{
	unsigned int indexBufferComponentIndex;

	RenderComponent* renderComponent = GetRenderComponent();

	if(renderComponent == NULL)
		return;

	if(!(renderComponent->GetRenderComponentFlag() & (1 << _componentType)))
	{
		MessageBox(NULL, "There is not an Index Buffer Component in this object! : BaseObject::AddIndexBufferComponent(INDEX_BUFFER_COMPONENTS _componentType, void* _data, unsigned int _size)", "Component System Error", 0);
		return;
	}

	unsigned int numRenderComponents = renderComponent->GetRenderComponents().size();
	for(unsigned int i = 0; i < numRenderComponents; ++i)
	{
		if(renderComponent->GetRenderComponents()[i].componentType == INDEX_BUFFER_RENDER_COMPONENT)
		{
			indexBufferComponentIndex = i;
			break;
		}
	}

	IndexBufferComponent* indexBufferComponent = ((IndexBufferComponent*)renderComponent->GetRenderComponents()[indexBufferComponentIndex].component);

	indexBufferComponent->AddIndexBufferComponent(_componentType, _data, _size);
}

void BaseObject::AddConstantBufferComponent(CONSTANT_BUFFER_COMPONENTS _componentType, void* _data, unsigned int _size)
{
	unsigned int constantBufferComponentIndex;

	RenderComponent* renderComponent = GetRenderComponent();

	if(renderComponent == NULL)
		return;

	if(!(renderComponent->GetRenderComponentFlag() & (1 << _componentType)))
	{
		MessageBox(NULL, "There is not a Constant Buffer Component in this object! : BaseObject::AddConstantBufferComponent(CONSTANT_BUFFER_COMPONENTS _componentType, void* _data, unsigned int _size)", "Component System Error", 0);
		return;
	}

	unsigned int numRenderComponents = renderComponent->GetRenderComponents().size();
	for(unsigned int i = 0; i < numRenderComponents; ++i)
	{
		if(renderComponent->GetRenderComponents()[i].componentType == CONSTANT_BUFFER_RENDER_COMPONENT)
		{
			constantBufferComponentIndex = i;
			break;
		}
	}

	ConstantBufferComponent* constantBufferComponent = ((ConstantBufferComponent*)renderComponent->GetRenderComponents()[constantBufferComponentIndex].component);

	constantBufferComponent->AddConstantBufferComponent(_componentType, _data, _size);
}

RenderComponent* BaseObject::GetRenderComponent()
{
	unsigned int renderComponentIndex;

	if(!(baseComponent->GetBaseComponentFlag() & (1 << RENDER_COMPONENT)))
	{
		MessageBox(NULL, "There is not a Render Component in this object! : BaseObject::GetRenderComponent()", "Component System Error", 0);
		return NULL;
	}

	for(unsigned int i = 0; i < baseComponent->GetBaseComponents().size(); ++i)
	{
		if(baseComponent->GetBaseComponents()[i].componentType == RENDER_COMPONENT)
		{
			renderComponentIndex = i;
			break;
		}
	}

	RenderComponent* tempRenderComponent = ((RenderComponent*)baseComponent->GetBaseComponents()[renderComponentIndex].component);

	return tempRenderComponent;
}

RenderComponent* BaseObject::LookAtVertexComponent()
{
	RenderComponent* renderComponent = GetRenderComponent();
	VertexBufferComponent* vertexBufferComponent;
	unsigned int vertexBufferComponentIndex;

	unsigned int numRenderComponents = renderComponent->GetRenderComponents().size();
	for(unsigned int i = 0; i < numRenderComponents; ++i)
	{
		if(renderComponent->GetRenderComponents()[i].componentType == VERTEX_BUFFER_RENDER_COMPONENT)
		{
			vertexBufferComponentIndex = i;
			break;
		}
	}

	vertexBufferComponent = ((VertexBufferComponent*)renderComponent->GetRenderComponents()[vertexBufferComponentIndex].component);

	return vertexBufferComponent;
}

RenderComponent* BaseObject::LookAtIndexComponent()
{
	RenderComponent* renderComponent = GetRenderComponent();
	IndexBufferComponent* indexBufferComponent;
	unsigned int indexBufferComponentIndex;

	unsigned int numRenderComponents = renderComponent->GetRenderComponents().size();
	for(unsigned int i = 0; i < numRenderComponents; ++i)
	{
		if(renderComponent->GetRenderComponents()[i].componentType == INDEX_BUFFER_RENDER_COMPONENT)
		{
			indexBufferComponentIndex = i;
			break;
		}
	}

	indexBufferComponent = ((IndexBufferComponent*)renderComponent->GetRenderComponents()[indexBufferComponentIndex].component);

	return indexBufferComponent;
}

RenderComponent* BaseObject::LookAtConstantComponent()
{
	RenderComponent* renderComponent = GetRenderComponent();
	ConstantBufferComponent* constantBufferComponent;
	unsigned int constantBufferComponentIndex;

	unsigned int numRenderComponents = renderComponent->GetRenderComponents().size();
	for(unsigned int i = 0; i < numRenderComponents; ++i)
	{
		if(renderComponent->GetRenderComponents()[i].componentType == CONSTANT_BUFFER_RENDER_COMPONENT)
		{
			constantBufferComponentIndex = i;
			break;
		}
	}

	constantBufferComponent = ((ConstantBufferComponent*)renderComponent->GetRenderComponents()[constantBufferComponentIndex].component);

	return constantBufferComponent;
}