#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include "../Game Objects/Component System/BaseComponent.h"
#include "../Game Objects/Component System/BaseComponent.h"

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

	void AddBaseComponent(BASE_COMPONENTS _component);

private:

	BaseComponent* baseComponent;
};

#endif