#ifndef COMPONENTSYSTEMDEFINITIONS_H
#define COMPONENTSYSTEMDEFINITIONS_H


class BaseComponent;
class Component;
enum BASE_COMPONENTS;
enum RENDER_COMPONENTS;
enum GAMEOBJECT_COMPONENTS;

struct BaseComponentStruct
{
	BASE_COMPONENTS componentType;
	Component* component;
};

struct RenderComponentStruct
{
	RENDER_COMPONENTS componentType;
	Component* component;
};

struct GameObjectComponentStruct
{
	GAMEOBJECT_COMPONENTS componentType;
	Component* component;
};

#endif