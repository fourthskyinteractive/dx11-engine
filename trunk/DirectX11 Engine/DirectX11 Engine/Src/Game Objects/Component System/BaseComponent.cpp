#include "BaseComponent.h"


BaseComponent::BaseComponent()
{

}

BaseComponent::BaseComponent(const BaseComponent& _dynamicObject)
{

}

BaseComponent::~BaseComponent()
{

}

void* BaseComponent::AddBaseComponent(BASE_COMPONENTS _component, BaseComponent* _componentToAdd)
{
	baseCompenentFlag |= (1 << _component);
}

void BaseComponent::RemoveBaseComponent(BASE_COMPONENTS _component)
{
	baseCompenentFlag &= ~(1 << _component);
}