#include "GameObjectComponent.h"


GameObjectComponent::GameObjectComponent()
{
	memset(&gameObjectComponentFlag, 0, sizeof(char)); 
}

GameObjectComponent::GameObjectComponent(const GameObjectComponent& _component)
{

}

GameObjectComponent::~GameObjectComponent()
{

}

void GameObjectComponent::AddGameObjectComponent(GAMEOBJECT_COMPONENTS _componentType, GameObjectComponent* _component)
{
	gameObjectComponentFlag |= (1 << _componentType);

	GameObjectComponentStruct componentToAdd;
	componentToAdd.component = _component;
	componentToAdd.componentType = _componentType;

	gameObjectComponents.push_back(componentToAdd);
}

void GameObjectComponent::RemoveGameObjectComponent(GAMEOBJECT_COMPONENTS _componentType)
{
	gameObjectComponentFlag &= ~(1 << _componentType);
}

Component* GameObjectComponent::GetAnimationComponent()
{
	for(unsigned int i = 0; i < gameObjectComponents.size(); ++i)
	{
		if(gameObjectComponents[i].componentType == ANINMATION_GAMEOBJECT_COMPONENT)
		{
			return gameObjectComponents[i].component;
		}
	}

	return NULL;
}