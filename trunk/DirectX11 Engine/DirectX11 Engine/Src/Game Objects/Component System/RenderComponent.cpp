#include "RenderComponent.h"


RenderComponent::RenderComponent()
{

}

RenderComponent::RenderComponent(const RenderComponent& _component)
{

}

RenderComponent::~RenderComponent()
{

}

void RenderComponent::AddRenderComponent(RENDER_COMPONENTS _component)
{
	renderComponentFlag |= (1 << _component);
}

void RenderComponent::RemoveRenderComponent(RENDER_COMPONENTS _component)
{
	renderComponentFlag &= ~(1 << _component);
}