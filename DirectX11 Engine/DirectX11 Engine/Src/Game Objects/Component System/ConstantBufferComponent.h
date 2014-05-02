#ifndef CONSTANTBUFFERCOMPONENT_H
#define CONSTANTBUFFERCOMPONENT_H

#include <vector>
using namespace std;

#include "RenderComponent.h"
#include "../../Renderer/D3D11Renderer.h"
#include "../../Renderer/ShaderManager.h"

enum CONSTANT_BUFFER_COMPONENTS{WORLD_MATRIX_COMPONENT, VIEW_MATRIX_COMPONENT, PROJECTION_MATRIX_COMPONENT, INVERSE_VIEW_MATRIX_COMPONENT, MISCELLANEOUS_COMPONENT};

struct ConstantComponent
{
	CONSTANT_BUFFER_COMPONENTS componentType;
	SHADER_TYPE associatedShader;
	unsigned int bufferSlot;
	unsigned int size;
	CComPtr<ID3D11Buffer> buffer;
	void* memoryAddress;
};

class ConstantBufferComponent : public RenderComponent
{
public:
	ConstantBufferComponent();
	ConstantBufferComponent(const ConstantBufferComponent& _vertexBufferComponent);
	~ConstantBufferComponent();

	void AddConstantBufferComponent(SHADER_TYPE _associatedShader, CONSTANT_BUFFER_COMPONENTS _component, void* _data, unsigned int _size, void* _memoryAddress);
	void RemoveConstantBufferComponent(CONSTANT_BUFFER_COMPONENTS _component);
	unsigned int GetNumberConstantBufferComponents(){return constantBufferComponents.size();}

	vector<ConstantComponent*> GetConstantBufferComponents(){return constantBufferComponents;}


private:
	char constantBufferComponentFlag;

	vector<ConstantComponent*> constantBufferComponents;
};

#endif