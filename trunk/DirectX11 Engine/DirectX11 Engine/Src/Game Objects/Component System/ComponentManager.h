#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "../../Game Objects/BaseObject.h"
#include "BaseComponent.h"
#include "RenderComponent.h"

class ComponentManager
{
public:
	ComponentManager();
	ComponentManager(const ComponentManager& _componentManager);
	~ComponentManager();

	static void AddBaseComponent(BaseObject* _baseObject, BASE_COMPONENTS _component);
	static void AddRenderComponent(BaseObject* _baseObject, RENDER_COMPONENTS _component);
	static void AddVertexBufferComponent(BaseObject* _baseObject, );
};
#endif