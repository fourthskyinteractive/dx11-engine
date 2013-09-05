#include "RenderComponent.h"


RenderComponent::RenderComponent()
{
	memset(&renderComponentFlag, 0, sizeof(char));
}

RenderComponent::RenderComponent(const RenderComponent& _component)
{

}

RenderComponent::~RenderComponent()
{

}

void RenderComponent::AddRenderComponent(RENDER_COMPONENTS _componentType, RenderComponent* _component)
{
	renderComponentFlag |= (1 << _componentType);

	RenderComponentStruct componentToAdd;
	componentToAdd.component = _component;
	componentToAdd.componentType = _componentType;

	renderComponents.push_back(componentToAdd);
}

void RenderComponent::RemoveRenderComponent(RENDER_COMPONENTS _componentType)
{
	renderComponentFlag &= ~(1 << _componentType);
}