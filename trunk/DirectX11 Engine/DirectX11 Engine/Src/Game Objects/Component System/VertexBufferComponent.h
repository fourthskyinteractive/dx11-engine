#ifndef VERTEXBUFFERCOMPONENT_H
#define VERTEXBUFFERCOMPONENT_H

#include <vector>
using namespace std;

#include "RenderComponent.h"
#include "../../Renderer/D3D11Renderer.h"

enum VERTEX_BUFFER_COMPONENTS{VERTEX_POSITION_COMPONENT, VERTEX_NORMAL_COMPONENT, VERTEX_TEXCOORD_COMPONENT, VERTEX_TANGENT_COMPONENT, VERTEX_BINORMAL_COMPONENT, VERTEX_ANIMATIONBLEND_COMPONENT};

struct VertexComponent
{
	VERTEX_BUFFER_COMPONENTS componentType;
	unsigned int stride;
	unsigned int totalSize;
	CComPtr<ID3D11Buffer> buffer;
};

class VertexBufferComponent : public RenderComponent
{
public:
	VertexBufferComponent();
	VertexBufferComponent(const VertexBufferComponent& _vertexBufferComponent);
	~VertexBufferComponent();

	void AddVertexBufferComponent(VERTEX_BUFFER_COMPONENTS _component, void* _data, unsigned int _stride, unsigned int _totalSize);
	void RemoveVertexBufferComponent(VERTEX_BUFFER_COMPONENTS _component);

	unsigned int GetNumberVertexBufferComponents(){return vertexBufferComponents.size();}

	vector<VertexComponent*> GetVertexBufferComponents(){return vertexBufferComponents;}

private:
	char vertexBufferComponentFlag;

	vector<VertexComponent*> vertexBufferComponents;
};

#endif