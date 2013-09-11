#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include "../Game Objects/Component System/BaseComponent.h"
#include "../Game Objects/Component System/RenderComponent.h"
#include "../Game Objects/Component System/VertexBufferComponent.h"
#include "../Game Objects/Component System/IndexBufferComponent.h"
#include "../Game Objects/Component System/ConstantBufferComponent.h"

#include <d3d11.h>
#include <d3d11shader.h>
#include <vector>
using namespace std;

class BaseObject
{
public:
	BaseObject();
	BaseObject(const BaseObject&);
	~BaseObject();

	void AddBaseComponent(BASE_COMPONENTS _componentType);
	void AddRenderComponent(RENDER_COMPONENTS _componentType);
	void AddVertexBufferComponent(VERTEX_BUFFER_COMPONENTS _componentType, void* _data, unsigned int _size);
	void AddIndexBufferComponent(INDEX_BUFFER_COMPONENTS _componentType, void* _data, unsigned int _size);
	void AddConstantBufferComponent(CONSTANT_BUFFER_COMPONENTS _componentType, void* _data, unsigned int _size);

	RenderComponent* GetRenderComponent();
	RenderComponent* LookAtVertexComponent();
	RenderComponent* LookAtIndexComponent();
	RenderComponent* LookAtConstantComponent();

private:

	BaseComponent* baseComponent;
};

#endif