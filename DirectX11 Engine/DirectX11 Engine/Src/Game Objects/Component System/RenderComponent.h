#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include <vector>
using namespace std;

#include "BaseComponent.h"
#include "ComponentSystemDefinitions.h"

enum RENDER_COMPONENTS{VERTEX_BUFFER_RENDER_COMPONENT, INDEX_BUFFER_RENDER_COMPONENT, CONSTANT_BUFFER_RENDER_COMPONENT, TEXTURE_RENDER_COMPONENT};

class RenderComponent : public BaseComponent
{
public:
	RenderComponent();
	RenderComponent(const RenderComponent& _component);
	~RenderComponent();

	void AddRenderComponent(RENDER_COMPONENTS _componentType, RenderComponent* _component);
	void RemoveRenderComponent(RENDER_COMPONENTS _componentType);

	char GetRenderComponentFlag(){return renderComponentFlag;}
	vector<RenderComponentStruct> GetRenderComponents(){return renderComponents;}

private:
	char renderComponentFlag;
	vector<RenderComponentStruct> renderComponents;
};

#endif