#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include <vector>
using namespace std;

#include "Component.h"
#include "ComponentSystemDefinitions.h"

enum RENDER_COMPONENTS{VERTEX_BUFFER_RENDER_COMPONENT, INDEX_BUFFER_RENDER_COMPONENT, CONSTANT_BUFFER_RENDER_COMPONENT};

class RenderComponent : public Component
{
public:
	RenderComponent();
	RenderComponent(const RenderComponent& _component);
	virtual ~RenderComponent();

	void AddRenderComponent(RENDER_COMPONENTS _componentType, RenderComponent* _component);
	void RemoveRenderComponent(RENDER_COMPONENTS _componentType);

	char GetRenderComponentFlag(){return renderComponentFlag;}
	vector<RenderComponentStruct> GetRenderComponents(){return renderComponents;}

	Component* GetVertexBufferRenderComponent();

	unsigned int GetNumberRenderComponents(){return renderComponents.size();}

private:
	char renderComponentFlag;
	vector<RenderComponentStruct> renderComponents;
};

#endif