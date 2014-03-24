#ifndef GAMEOBJECTCOMPONENT_H
#define GAMEOBJECTCOMPONENT_H

#include <vector>
using namespace std;

#include "Component.h"
#include "ComponentSystemDefinitions.h"

enum GAMEOBJECT_COMPONENTS{ANINMATION_GAMEOBJECT_COMPONENT};

class GameObjectComponent : public Component
{
public:
	GameObjectComponent();
	GameObjectComponent(const GameObjectComponent& _component);
	virtual ~GameObjectComponent();

	void AddGameObjectComponent(GAMEOBJECT_COMPONENTS _componentType, GameObjectComponent* _component);
	void RemoveGameObjectComponent(GAMEOBJECT_COMPONENTS _componentType);

	char GetGameObjectComponentFlag(){return gameObjectComponentFlag;}
	vector<GameObjectComponentStruct> GetRenderComponents(){return gameObjectComponents;}

	Component* GetAnimationComponent();

	unsigned int GetNumberGameObjectComponents(){return gameObjectComponents.size();}

private:
	char gameObjectComponentFlag;
	vector<GameObjectComponentStruct> gameObjectComponents;
};

#endif