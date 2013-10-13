#ifndef SCREENSPACEOBJECT_H
#define SCREENSPACEOBJECT_H

#include "BaseObject.h"

class ScreenSpaceObject : public BaseObject
{
public:
	ScreenSpaceObject();
	ScreenSpaceObject(const ScreenSpaceObject& _screenSpaceObject);
	~ScreenSpaceObject();

	void UpdateShaderConstantBuffers();
	void BindRenderComponents();
	void FinalizeObject();
	void Render();
	void Update(float _dt);
	void Destroy();

	void SetWorldMatrix(XMFLOAT4X4 _worldMatrix){worldMatrix = _worldMatrix;}

private:
	XMFLOAT4X4 worldMatrix;

};

#endif