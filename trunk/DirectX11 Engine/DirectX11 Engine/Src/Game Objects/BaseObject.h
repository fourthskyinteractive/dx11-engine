#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include <d3d11.h>
#include <d3d11shader.h>

class BaseObject
{
public:
	BaseObject();
	BaseObject(const BaseObject&);
	~BaseObject();
};

#endif