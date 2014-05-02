#include "ConstantBufferComponent.h"
#include "../../Utility/Misc/DebugOutput.h"

ConstantBufferComponent::ConstantBufferComponent()
{
	memset(&constantBufferComponentFlag, 0, sizeof(char));
}

ConstantBufferComponent::ConstantBufferComponent(const ConstantBufferComponent& _vertexBufferComponent)
{

}

ConstantBufferComponent::~ConstantBufferComponent()
{
	for(unsigned int i = 0; i < constantBufferComponents.size(); ++i)
	{
		delete constantBufferComponents[i];
		constantBufferComponents[i] = NULL;
	}
}

void ConstantBufferComponent::AddConstantBufferComponent(SHADER_TYPE _associatedShader, CONSTANT_BUFFER_COMPONENTS _component, void* _data, unsigned int _size, void* _memoryAddress)
{
	constantBufferComponentFlag |= (1 << _component);

	ConstantComponent* newConstantComponent = new ConstantComponent;
	newConstantComponent->componentType = _component;
	newConstantComponent->size = _size;
	newConstantComponent->memoryAddress = _memoryAddress;
	newConstantComponent->associatedShader = _associatedShader;

	unsigned int bufferSlot = 0;
	for(unsigned int i = 0; i < constantBufferComponents.size(); ++i)
	{
		if(constantBufferComponents[i]->associatedShader == _associatedShader)
		{
			bufferSlot++;
		}
	}
	newConstantComponent->bufferSlot = bufferSlot;

	D3D11_BUFFER_DESC cBufferDesc;
	ZeroMemory(&cBufferDesc, sizeof(D3D11_BUFFER_DESC));
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDesc.ByteWidth = _size;
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA cData;
	ZeroMemory(&cData, sizeof(D3D11_SUBRESOURCE_DATA));
	cData.pSysMem = _data;

	HRESULT hr = D3D11Renderer::d3dDevice->CreateBuffer(&cBufferDesc, &cData, &newConstantComponent->buffer);

	if(hr != S_OK)
	{
		//MessageBox(NULL, "Constant Buffer Creation Failed : ConstantBufferComponent::AddConstantBufferComponent(CONSTANT_BUFFER_COMPONENTS _component, void* _data, unsigned int _size)", "Constant Buffer Component Error", 0);
		DebugOutput::Print(DebugOutput::RENDER_MSG, DebugOutput::FAILURE_CREATE, "ConstantBuffer");
	}

	constantBufferComponents.push_back(newConstantComponent);
}

void ConstantBufferComponent::RemoveConstantBufferComponent(CONSTANT_BUFFER_COMPONENTS _component)
{
	if(constantBufferComponentFlag & (1 << _component))
	{
		constantBufferComponentFlag &= ~(1 << _component);

		for (vector<ConstantComponent*>::iterator iter = constantBufferComponents.begin(); iter != constantBufferComponents.end(); iter++)
		{
			if((*iter)->componentType == _component)
			{
				constantBufferComponents.erase(iter);
				break;
			}
		}	
	}
}