#ifndef BASECOMPONENT_H
#define BASECOMPONENT_H

#include "Component.h"
#include "ComponentSystemDefinitions.h"

#include <vector>
using namespace std;

enum BASE_COMPONENTS{RENDER_COMPONENT, GAMEOBJECT_COMPONENT};

class BaseComponent : public Component
{
public:
	BaseComponent();
	BaseComponent(const BaseComponent& _baseComponent);
	virtual ~BaseComponent();

	void AddBaseComponent(BASE_COMPONENTS _componentType, Component* _component);
	void RemoveBaseComponent(BASE_COMPONENTS _componentType);

	char GetBaseComponentFlag(){return baseCompenentFlag;}
	vector<BaseComponentStruct> GetBaseComponents(){return baseComponents;}

private:
	char baseCompenentFlag;

	vector<BaseComponentStruct> baseComponents;
};

#endif