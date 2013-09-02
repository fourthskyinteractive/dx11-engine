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

void BaseObject::AddBaseComponent(BASE_COMPONENTS _component)
{
	BaseComponent* componentToAdd;

	switch (_component)
	{
	case RENDER_COMPONENT:
		componentToAdd = new RenderComponent();
		baseComponent->AddBaseComponent(RENDER_COMPONENT, componentToAdd);
		break;
	default:
		break;
	}

	for(unsigned int i = 0; i < baseComponent->baseComponents.size(); ++i)
	{
		if(baseComponent->baseComponents[i].componentType == RENDER_COMPONENT)
		{
			((RenderComponent*)baseComponent->baseComponents[i].component)->AddRenderComponent(VERTEX_BUFFER_RENDER_COMPONENT);

			int poop = 0;
		}
	}
}