#ifndef VERTEXBUFFERCOMPONENT_H
#define VERTEXBUFFERCOMPONENT_H

#include <vector>
using namespace std;

#include "RenderComponent.h"
#include "../../Renderer/D3D11Renderer.h"

enum VERTEX_BUFFER_COMPONENTS{VERTEX_POSITION_COMPONENT, VERTEX_COLOR_COMPONENT, VERTEX_NORMAL_COMPONENT, VERTEX_TEXCOORD_COMPONENT};

struct VertexComponent
{
	VERTEX_BUFFER_COMPONENTS componentType;
	unsigned int size;
	CComPtr<ID3D11Buffer> buffer;
};

class VertexBufferComponent : public RenderComponent
{
public:
	VertexBufferComponent();
	VertexBufferComponent(const VertexBufferComponent& _vertexBufferComponent);
	~VertexBufferComponent();

	void AddVertexBufferComponent(VERTEX_BUFFER_COMPONENTS _component, void* _data, unsigned int _size);
	void RemoveVertexBufferComponent(VERTEX_BUFFER_COMPONENTS _component);

private:
	char vertexBufferComponentFlag;

	vector<VertexComponent*> vertexBufferComponents;
};

#endif