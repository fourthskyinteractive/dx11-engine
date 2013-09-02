#include "BaseObject.h"
#include "../Renderer/D3D11Renderer.h"
#include "../Game/Game.h"

BaseObject::BaseObject()
{

}

BaseObject::BaseObject(const BaseObject& _baseObject)
{

}

BaseObject::~BaseObject()
{

}

void BaseObject::AddBaseComponent(BASE_COMPONENTS _component)
{
	switch (_component)
	{
	case RENDER_COMPONENT:
		baseComponent = new BaseComponent();
		baseComponent->AddBaseComponent(RENDER_COMPONENT);
		break;
	default:
		break;
	}
}