#include "DynamicObject.h"
#include "../Renderer/D3D11Renderer.h"

DynamicObject::DynamicObject()
{
	ZeroMemory(&componentsBitFlag, sizeof(char));
}

DynamicObject::DynamicObject(const DynamicObject& _dynamicObject)
{

}

DynamicObject::~DynamicObject()
{

}


void DynamicObject::Render()
{

}

void DynamicObject::Update(float _dt)
{

}