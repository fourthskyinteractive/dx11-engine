#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include "BaseObject.h"

enum OBJECT_TYPE{STATIC_OBJECT, DYNAMIC_OBJECT};

struct GameObject
{	
	BaseObject* object;
	bool renderOn;

	GameObject() : object(NULL), renderOn(true){};
};

struct BlendingInformation
{
	XMFLOAT2 joint1;
	XMFLOAT2 joint2;
	XMFLOAT2 joint3;
	XMFLOAT2 joint4;
};

class ObjectManager
{
public:
	ObjectManager();
	ObjectManager(const ObjectManager&){};
	~ObjectManager();

	static void Initialize();
	static void AddObject(OBJECT_TYPE _objectType, string _modelPath, XMFLOAT4X4 _worldMatrix);
	static void UpdateObjects(float _dt);
	static void RenderObjects();

	static vector<GameObject*> staticObjects;
	static vector<GameObject*> dynamicObjects;
	static BaseObject* finalPassObject;
};
#endif