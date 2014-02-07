#ifndef WORLDOBJECT_H
#define WORLDOBJECT_H

#include "BaseObject.h"

class WorldObject : public BaseObject
{
public:
	WorldObject();
	WorldObject(const WorldObject& _worldObject);
	~WorldObject();

	void UpdateShaderConstantBuffers();
	void BindRenderComponents();
	void FinalizeObject();
	void Render();
	void Update(float _dt);
	void Destroy();

	void SetWorldMatrix(XMFLOAT4X4 _worldMatrix){worldMatrix = _worldMatrix;}
	XMFLOAT4X4 GetWorldMatrixF(){return worldMatrix;}
	void* GetWorldMatrixP(){return &worldMatrix;}

private:
	XMFLOAT4X4 worldMatrix;

};

#endif