#ifndef BASECOMPONENT_H
#define BASECOMPONENT_H

enum BASE_COMPONENTS{RENDER_COMPONENT};

enum CONSTANTBUFFERCOMPONENT{WORLDMATRIX_CONSTANT_COMPONENT, VIEWMATRIX_CONSTANT_COMPONENT, PROJECTIONMATRIX_CONSTANT_COMPONENT};
enum TEXTURECOMPONENTS{DIFFUSE_TEXTURE_COMPONENT, NORMAL_TEXTURE_COMPONENT, SPECULAR_TEXTURE_COMPONENT};

class BaseComponent
{
public:
	BaseComponent();
	BaseComponent(const BaseComponent& _dynamicObject);
	~BaseComponent();

	void AddBaseComponent(BASE_COMPONENTS _component);
	void RemoveBaseComponent(BASE_COMPONENTS _component);

private:
	char baseCompenentFlag;
};

#endif