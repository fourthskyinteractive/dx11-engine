#include "BaseComponent.h"


BaseComponent::BaseComponent()
{
	memset(&baseCompenentFlag, 0, sizeof(char));
}

BaseComponent::BaseComponent(const BaseComponent& _dynamicObject)
{

}

BaseComponent::~BaseComponent()
{

}

void BaseComponent::AddBaseComponent(BASE_COMPONENTS _component, BaseComponent* _componentToAdd)
{
	baseCompenentFlag |= (1 << _component);

	BaseComponentStruct componentToAdd;
	componentToAdd.component = _componentToAdd;
	componentToAdd.componentType = _component;

	baseComponents.push_back(componentToAdd);
}

void BaseComponent::RemoveBaseComponent(BASE_COMPONENTS _component)
{
	baseCompenentFlag &= ~(1 << _component);
}