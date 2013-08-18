#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include "BaseComponent.h"

enum RENDER_COMPONENTS{VERTEX_BUFFER_COMPONENT, INDEX_BUFFER_COMPONENT, CONSTANT_BUFFER_COMPONENT, TEXTURE_COMPONENT, ANIMATION_COMPONENT};

class RenderComponent : public BaseComponent
{
public:
	RenderComponent();
	RenderComponent(const RenderComponent& _component);
	~RenderComponent();

private:
	char renderComponentFlag;

	void AddRenderComponent(RENDER_COMPONENTS _component);
	void RemoveRenderComponent(RENDER_COMPONENTS _component);
};

#endif