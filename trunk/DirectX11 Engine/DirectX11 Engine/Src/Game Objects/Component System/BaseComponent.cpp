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
	for(unsigned int i = 0; i < baseComponents.size(); ++i)
	{
		delete baseComponents[i].component;
		baseComponents[i].component = NULL;
	}
}

void BaseComponent::AddBaseComponent(BASE_COMPONENTS _componentType, BaseComponent* _component)
{
	baseCompenentFlag |= (1 << _componentType);

	BaseComponentStruct componentToAdd;
	componentToAdd.component = _component;
	componentToAdd.componentType = _componentType;

	baseComponents.push_back(componentToAdd);
}

void BaseComponent::RemoveBaseComponent(BASE_COMPONENTS _componentType)
{
	baseCompenentFlag &= ~(1 << _componentType);
}