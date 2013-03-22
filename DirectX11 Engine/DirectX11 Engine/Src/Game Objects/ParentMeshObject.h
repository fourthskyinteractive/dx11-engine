#ifndef PARENTMESHOBJECT_H
#define PARENTMESHOBJECT_H


#include "../Game/Definitions.h"


#include <Windows.h>
#include <d3d11.h>
#include <d3d11shader.h>
#include <list>
using namespace std;



class ChildMeshObject;

class ParentMeshObject
{
private:
	XMFLOAT4X4	worldMatrix;
	XMFLOAT3	position;
	XMFLOAT3	rotation;
	XMFLOAT3	scale;

	list<ChildMeshObject*> children;

public:
	ParentMeshObject();
	ParentMeshObject(const ParentMeshObject& _parentMeshObject);
	~ParentMeshObject();


	XMFLOAT4X4 GetWorldMatrixF(){return worldMatrix;}
	XMMATRIX GetWorldMatrixM(){return XMLoadFloat4x4(&worldMatrix);}
	XMFLOAT3 GetPosition(){return position;}
	XMFLOAT3 GetRotation(){return rotation;}
	XMFLOAT3 GetScale(){return scale;}

	
	void SetWorldMatrixF(XMFLOAT4X4 _worldMatrix){worldMatrix = _worldMatrix;}
	void SetPosition(XMFLOAT3 _position){;}
	void SetRotation(XMFLOAT3 _rotation){rotation = _rotation;}
	void SetScale(XMFLOAT3 _scale){scale = _scale;}
	void Initialize(XMFLOAT3 _position, XMFLOAT3 _scale, XMFLOAT3 _rotation);
	void AddChild(ChildMeshObject* _child);
	void Render();

	void UpdateWorldMatrix();
};
#endif
