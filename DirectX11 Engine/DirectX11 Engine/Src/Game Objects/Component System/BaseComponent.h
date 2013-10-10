#ifndef BASECOMPONENT_H
#define BASECOMPONENT_H

#include "ComponentSystemDefinitions.h"

#include <vector>
using namespace std;

enum BASE_COMPONENTS{RENDER_COMPONENT};

enum CONSTANTBUFFERCOMPONENT{WORLDMATRIX_CONSTANT_COMPONENT, VIEWMATRIX_CONSTANT_COMPONENT, PROJECTIONMATRIX_CONSTANT_COMPONENT};
enum TEXTURECOMPONENTS{DIFFUSE_TEXTURE_COMPONENT, NORMAL_TEXTURE_COMPONENT, SPECULAR_TEXTURE_COMPONENT};

class BaseComponent
{
public:
	BaseComponent();
	BaseComponent(const BaseComponent& _baseComponent);
	virtual ~BaseComponent();

	void AddBaseComponent(BASE_COMPONENTS _componentType, BaseComponent* _component);
	void RemoveBaseComponent(BASE_COMPONENTS _componentType);


	char GetBaseComponentFlag(){return baseCompenentFlag;}
	vector<BaseComponentStruct> GetBaseComponents(){return baseComponents;}

private:
	char baseCompenentFlag;

	vector<BaseComponentStruct> baseComponents;
};

#endif