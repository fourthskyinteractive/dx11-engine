#ifndef INDEXBUFFERCOMPONENT_H
#define INDEXBUFFERCOMPONENT_H

#include <vector>
using namespace std;

#include "RenderComponent.h"
#include "../../Renderer/D3D11Renderer.h"

enum INDEX_BUFFER_COMPONENTS{INDICIES_COMPONENT};

struct IndexComponent
{
	INDEX_BUFFER_COMPONENTS componentType;
	unsigned int size;
	CComPtr<ID3D11Buffer> buffer;
};

class IndexBufferComponent : public RenderComponent
{
public:
	IndexBufferComponent();
	IndexBufferComponent(const IndexBufferComponent& _vertexBufferComponent);
	~IndexBufferComponent();

	void AddIndexBufferComponent(INDEX_BUFFER_COMPONENTS _component, void* _data, unsigned int _size);
	void RemoveIndexBufferComponent(INDEX_BUFFER_COMPONENTS _component);

	vector<IndexComponent*> GetIndexBufferComponents(){return indexBufferComponents;}
	unsigned int GetNumberIndexBufferComponents(){return indexBufferComponents.size();}

private:
	char indexBufferComponentFlag;

	vector<IndexComponent*> indexBufferComponents;
};

#endif