#ifndef COMPONENTSYSTEMDEFINITIONS_H
#define COMPONENTSYSTEMDEFINITIONS_H


class BaseComponent;
enum BASE_COMPONENTS;
enum RENDER_COMPONENTS;

struct BaseComponentStruct
{
	BASE_COMPONENTS componentType;
	BaseComponent* component;
};

struct RenderComponentStruct
{
	RENDER_COMPONENTS componentType;
	BaseComponent* component;
};

#endif