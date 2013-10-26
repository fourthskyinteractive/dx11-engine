#include "BaseObject.h"
#include "../Renderer/D3D11Renderer.h"
#include "../Game/Game.h"
#include "../Utility/Model Loaders/FBXLoader.h"
#include "../Renderer/TextureManager.h"

BaseObject::BaseObject()
{
	baseComponent = new BaseComponent();
	numVertexComponents = 0;
	renderable = false;
	useComputeShader = false;

	renderDataMembers = new DX11RenderDataMembers();
	vertexBufferInformationForBinding = new BuffersForBinding();

	memset(renderDataMembers, 0, sizeof(DX11RenderDataMembers));
	memset(vertexBufferInformationForBinding, 0, sizeof(BuffersForBinding));
}

BaseObject::BaseObject(const BaseObject& _baseObject)
{

}

BaseObject::~BaseObject()
{
	delete renderDataMembers;
	renderDataMembers = NULL;

	delete vertexBufferInformationForBinding;
	vertexBufferInformationForBinding = NULL;

	delete baseComponent;
	baseComponent = NULL;
}

void BaseObject::AddBaseComponent(BASE_COMPONENTS _componentType)
{
	Component* componentToAdd;

	switch (_componentType)
	{
	case RENDER_COMPONENT:
		componentToAdd = new RenderComponent();
		baseComponent->AddBaseComponent(RENDER_COMPONENT, componentToAdd);
		renderComponent = (RenderComponent*)componentToAdd;
		break;
	default:
		break;
	}
}

void BaseObject::AddRenderComponent(RENDER_COMPONENTS _componentType)
{
	RenderComponent* componentToAdd;
	RenderComponent* renderComponent;

	if(!(baseComponent->GetBaseComponentFlag() & (1 << RENDER_COMPONENT)))
	{
		MessageBox(NULL, "There is not a Render Component in this object!", "Component System Error", 0);
		return;
	}

	renderComponent = GetRenderComponent();

	switch (_componentType)
	{
	case VERTEX_BUFFER_RENDER_COMPONENT:
		componentToAdd = new VertexBufferComponent();
		renderComponent->AddRenderComponent(_componentType, componentToAdd);
		vertexComponent = (VertexBufferComponent*)componentToAdd;
		break;
	case INDEX_BUFFER_RENDER_COMPONENT:
		componentToAdd = new IndexBufferComponent();
		renderComponent->AddRenderComponent(_componentType, componentToAdd);
		indexComponent = (IndexBufferComponent*)componentToAdd;
		break;
	case CONSTANT_BUFFER_RENDER_COMPONENT:
		componentToAdd = new ConstantBufferComponent();
		renderComponent->AddRenderComponent(_componentType, componentToAdd);
		constantComponent = (ConstantBufferComponent*)componentToAdd;
		break;
	default:
		break;
	}

// 	RenderComponent* parentSlice = dynamic_cast<RenderComponent*>(componentToAdd);
// 	memcpy(parentSlice, renderComponent, sizeof(RenderComponent));

}

void BaseObject::AddVertexBufferComponent(VERTEX_BUFFER_COMPONENTS _componentType, void* _data, unsigned int _stride, unsigned int _totalSize)
{
	unsigned int vertexBufferComponentIndex;

	RenderComponent* renderComponent = GetRenderComponent();

	if(renderComponent == NULL)
		return;

	if(!(renderComponent->GetRenderComponentFlag() & (1 << VERTEX_BUFFER_RENDER_COMPONENT)))
	{
		MessageBox(NULL, "There is not a Vertex Buffer Component in this object!", "Component System Error", 0);
		return;
	}

	unsigned int numRenderComponents = renderComponent->GetRenderComponents().size();
	for(unsigned int i = 0; i < numRenderComponents; ++i)
	{
		if(renderComponent->GetRenderComponents()[i].componentType == VERTEX_BUFFER_RENDER_COMPONENT)
		{
			vertexBufferComponentIndex = i;
			break;
		}
	}

	VertexBufferComponent* vertexBufferComponent = ((VertexBufferComponent*)renderComponent->GetRenderComponents()[vertexBufferComponentIndex].component);

	vertexBufferComponent->AddVertexBufferComponent(_componentType, _data, _stride, _totalSize);
	numVertexComponents++;
}

void BaseObject::AddIndexBufferComponent(INDEX_BUFFER_COMPONENTS _componentType, void* _data, unsigned int _stride, unsigned int _totalSize)
{
	numIndices = (_totalSize / _stride);
	unsigned int indexBufferComponentIndex;

	RenderComponent* renderComponent = GetRenderComponent();

	if(renderComponent == NULL)
		return;

	if(!(renderComponent->GetRenderComponentFlag() & (1 << INDEX_BUFFER_RENDER_COMPONENT)))
	{
		MessageBox(NULL, "There is not an Index Buffer Component in this object! : BaseObject::AddIndexBufferComponent(INDEX_BUFFER_COMPONENTS _componentType, void* _data, unsigned int _size)", "Component System Error", 0);
		return;
	}

	unsigned int numRenderComponents = renderComponent->GetRenderComponents().size();
	for(unsigned int i = 0; i < numRenderComponents; ++i)
	{
		if(renderComponent->GetRenderComponents()[i].componentType == INDEX_BUFFER_RENDER_COMPONENT)
		{
			indexBufferComponentIndex = i;
			break;
		}
	}

	IndexBufferComponent* indexBufferComponent = ((IndexBufferComponent*)renderComponent->GetRenderComponents()[indexBufferComponentIndex].component);

	indexBufferComponent->AddIndexBufferComponent(_componentType, _data, _totalSize);
}

void BaseObject::AddConstantBufferComponent(CONSTANT_BUFFER_COMPONENTS _componentType, void* _data, unsigned int _totalSize, void* (*_updateFunctionPointer)())
{
	unsigned int constantBufferComponentIndex;

	RenderComponent* renderComponent = GetRenderComponent();

	if(renderComponent == NULL)
		return;

	if(!(renderComponent->GetRenderComponentFlag() & (1 << CONSTANT_BUFFER_RENDER_COMPONENT)))
	{
		MessageBox(NULL, "There is not a Constant Buffer Component in this object! : BaseObject::AddConstantBufferComponent(CONSTANT_BUFFER_COMPONENTS _componentType, void* _data, unsigned int _size)", "Component System Error", 0);
		return;
	}

	unsigned int numRenderComponents = renderComponent->GetRenderComponents().size();
	for(unsigned int i = 0; i < numRenderComponents; ++i)
	{
		if(renderComponent->GetRenderComponents()[i].componentType == CONSTANT_BUFFER_RENDER_COMPONENT)
		{
			constantBufferComponentIndex = i;
			break;
		}
	}

	ConstantBufferComponent* constantBufferComponent = ((ConstantBufferComponent*)renderComponent->GetRenderComponents()[constantBufferComponentIndex].component);

	constantBufferComponent->AddConstantBufferComponent(_componentType, _data, _totalSize, _updateFunctionPointer);
}

void BaseObject::AddComputeShaderBuffer(void* _data, unsigned int _stride, unsigned int _totalSize, COMPUTEBUFFERTYPE _bufferType)
{
	HRESULT hr;
	CComPtr<ID3D11Buffer> dataBuffer;
	CComPtr<ID3D11ShaderResourceView> srv;
	CComPtr<ID3D11UnorderedAccessView> uav;

	//Creating the buffer that I will use to make the unordered access view and the shader resource view.
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.ByteWidth = _totalSize;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = _stride;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = _data;
	hr = D3D11Renderer::d3dDevice->CreateBuffer(&bufferDesc, &initData, &dataBuffer);

	if(hr == S_FALSE)
	{
		MessageBox(NULL, "Error Creating The Compute Shader Buffer! : BaseObject::AddComputeShaderBuffer(void* _data, unsigned int _stride, unsigned int _totalSize)", "Compute Buffer Error", 0);
		return;
	}

	//Creating the Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC sBufferDesc;	
	ZeroMemory(&sBufferDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	sBufferDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	sBufferDesc.BufferEx.FirstElement = 0;
	sBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	sBufferDesc.BufferEx.NumElements = bufferDesc.ByteWidth / bufferDesc.StructureByteStride;

	hr = D3D11Renderer::d3dDevice->CreateShaderResourceView(dataBuffer, &sBufferDesc, &srv);

	if(hr == S_FALSE)
	{
		MessageBox(NULL, "Error Creating The Compute Shader SRV! : BaseObject::AddComputeShaderBuffer(void* _data, unsigned int _stride, unsigned int _totalSize)", "Compute Buffer Error", 0);
		return;
	}

	//Creating the Unordered Access View
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavBufferDesc;

	ZeroMemory(&uavBufferDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavBufferDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavBufferDesc.Buffer.FirstElement = 0;
	uavBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavBufferDesc.Buffer.NumElements = bufferDesc.ByteWidth / bufferDesc.StructureByteStride;

	hr = D3D11Renderer::d3dDevice->CreateUnorderedAccessView(dataBuffer, &uavBufferDesc, &uav);

	if(hr == S_FALSE)
	{
		MessageBox(NULL, "Error Creating The Compute Shader UAV! : BaseObject::AddComputeShaderBuffer(void* _data, unsigned int _stride, unsigned int _totalSize)", "Compute Buffer Error", 0);
		return;
	}

	renderDataMembers->computeShaderBuffers.computeBufferType.push_back(_bufferType);
	renderDataMembers->computeShaderBuffers.computeDataBuffers.push_back(dataBuffer);
	renderDataMembers->computeShaderBuffers.computeSRVs.push_back(srv);
	renderDataMembers->computeShaderBuffers.computeUAVs.push_back(uav);
	
	if(!useComputeShader)
		useComputeShader = true;
}

void BaseObject::AddComputeShaderBuffer(CComPtr<ID3D11Buffer> _buffer, unsigned int _stride, unsigned int _totalSize)
{

}

// void BaseObject::AddComputeShaderBuffer(CComPtr<ID3D11ShaderResourceView> _srv, unsigned int _stride = 0, unsigned int _totalSize = 0)
// {
// 	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
// 	_srv->GetDesc(&desc);
// 
// 	
// }
// 
// void BaseObject::AddComputeShaderBuffer(CComPtr<ID3D11UnorderedAccessView> _uav, unsigned int _stride = 0, unsigned int _totalSize = 0)
// {
// 
// }

void BaseObject::AddTexture(WCHAR* _filePath)
{
	textureIndices.push_back(TextureManager::AddTexture(D3D11Renderer::d3dDevice, _filePath));
}

void BaseObject::AddTexture(CComPtr<ID3D11ShaderResourceView> _shaderResourceView)
{
	textureIndices.push_back(TextureManager::AddTexture(_shaderResourceView));
}
		
RenderComponent* BaseObject::GetRenderComponent()
{
	if(renderComponent)
		return renderComponent;
	else
	{
		MessageBox(NULL, "There is not a Render Component in this object! : BaseObject::GetRenderComponent()", "Component System Error", 0);
		return NULL;
	}
}

VertexBufferComponent* BaseObject::GetVertexBufferComponent()
{
	if(renderComponent)
	{
		if(vertexComponent)
			return vertexComponent;
		else
		{
			MessageBox(NULL, "There is not a Vertex Buffer Component in this object! : BaseObject::GetVertexBufferComponent()", "Component System Error", 0);
			return NULL;
		}
	}
	else
	{
		MessageBox(NULL, "There is not a Render Component in this object! : BaseObject::GetVertexBufferComponent()", "Component System Error", 0);
		return NULL;
	}
}

IndexBufferComponent* BaseObject::GetIndexBufferComponent()
{
	if(renderComponent)
	{
		if(indexComponent)
			return indexComponent;
		else
		{
			MessageBox(NULL, "There is not a Index Buffer Component in this object! : BaseObject::GetIndexBufferComponent()", "Component System Error", 0);
			return NULL;
		}
	}
	else
	{
		MessageBox(NULL, "There is not a Render Component in this object! : BaseObject::GetIndexBufferComponent()", "Component System Error", 0);
		return NULL;
	}
}

ConstantBufferComponent* BaseObject::GetConstantBufferComponent()
{
	if(renderComponent)
	{
		if(constantComponent)
			return constantComponent;
		else
		{
			MessageBox(NULL, "There is not a Constant Buffer Component in this object! : BaseObject::GetConstantBufferComponent()", "Component System Error", 0);
			return NULL;
		}
	}
	else
	{
		MessageBox(NULL, "There is not a Render Component in this object! : BaseObject::GetConstantBufferComponent()", "Component System Error", 0);
		return NULL;
	}
}
void BaseObject::LoadModel(char* _filePath, ModelData& _modelData)
{
	FBXLoader::LoadFBX(_filePath, _modelData);
}

void BaseObject::UpdateShaderConstantBuffers()
{
	
}

void BaseObject::BindRenderComponents()
{
	for(unsigned int i = 0; i < textureIndices.size(); ++i)
	{
		CComPtr<ID3D11ShaderResourceView> shaderView = TextureManager::GetTexture(textureIndices[i]);
		D3D11Renderer::d3dImmediateContext->PSSetShaderResources(i, 1, &shaderView.p);
	}

	//TODO:: Possibly move this somewhere else
	if(useComputeShader)
	{
		//D3D11Renderer::d3dImmediateContext->CSSetUnorderedAccessViews(0, renderDataMembers->computeShaderBuffers.computeUAVs.size(), &renderDataMembers->computeShaderBuffers.computeUAVs[0].p, NULL);

		//CComPtr<ID3D11UnorderedAccessView> nullUAV = NULL;
		//D3D11Renderer::d3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, NULL);

		//Set the G Buffer Textures
		for(unsigned int i = 0; i < textureIndices.size(); ++i)
		{
			CComPtr<ID3D11ShaderResourceView> shaderView = TextureManager::GetTexture(textureIndices[i]);
			D3D11Renderer::d3dImmediateContext->CSSetShaderResources(i, 1, &shaderView.p);
		}

		//Set the Lighting Information SRV
		D3D11Renderer::d3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &D3D11Renderer::backBufferUAV.p, NULL);

		D3D11Renderer::d3dImmediateContext->Dispatch(64, 64, 1);

		D3D11Renderer::d3dImmediateContext->VSSetShaderResources(0, renderDataMembers->computeShaderBuffers.computeSRVs.size(), &renderDataMembers->computeShaderBuffers.computeSRVs[0].p);
	}
}

void BaseObject::SetShaders(int _vertexShader, int _geometryShader, int _pixelShader, int _computeShader)
{
	if(_vertexShader < (int)ShaderManager::vertexShaders.size() && _vertexShader > -1)
	{
		renderDataMembers->vertexShader = ShaderManager::vertexShaders[_vertexShader].shader;
		renderDataMembers->vertexBufferBytes = ShaderManager::vertexShaders[_vertexShader].bufferBytes;
		renderDataMembers->vertexBufferBiteWidth = ShaderManager::vertexShaders[_vertexShader].byteWidth;
	}

	if(_geometryShader < (int)ShaderManager::geometryShaders.size() && _geometryShader > -1)
	{
		renderDataMembers->geometryShader = ShaderManager::geometryShaders[_geometryShader].shader;
	}

	if(_pixelShader < (int)ShaderManager::pixelShaders.size() && _pixelShader > -1)
	{
		renderDataMembers->pixelShader = ShaderManager::pixelShaders[_pixelShader].shader;
	}

	if(_computeShader < (int)ShaderManager::computeShaders.size() && _computeShader > -1)
	{
		renderDataMembers->computeShader = ShaderManager::computeShaders[_computeShader].shader;
	}
}

void BaseObject::FinalizeObject()
{
	HRESULT hr = 0;
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = -0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = D3D11Renderer::d3dDevice->CreateSamplerState(&samplerDesc, &renderDataMembers->samplerState);

	if(hr != S_OK)
	{
		MessageBox(NULL, "There is a problem with the sampler state! : BaseObject::FinalizeObject()", "Sampler State Problem", 0);
	}
}

void BaseObject::Render()
{
	if(!renderable)
		return;

	D3D11Renderer::ContextClearState(D3D11Renderer::d3dImmediateContext);
}

void BaseObject::Update(float _dt)
{
	if(!renderable)
		return;

}

void BaseObject::Destroy()
{

}

RenderComponent* BaseObject::LookAtVertexComponent()
{
	VertexBufferComponent* vertexBufferComponent;
	unsigned int vertexBufferComponentIndex;

	unsigned int numRenderComponents = renderComponent->GetRenderComponents().size();
	for(unsigned int i = 0; i < numRenderComponents; ++i)
	{
		if(renderComponent->GetRenderComponents()[i].componentType == VERTEX_BUFFER_RENDER_COMPONENT)
		{
			vertexBufferComponentIndex = i;
			break;
		}
	}

	vertexBufferComponent = ((VertexBufferComponent*)renderComponent->GetRenderComponents()[vertexBufferComponentIndex].component);

	return vertexBufferComponent;
}

RenderComponent* BaseObject::LookAtIndexComponent()
{
	IndexBufferComponent* indexBufferComponent;
	unsigned int indexBufferComponentIndex;

	unsigned int numRenderComponents = renderComponent->GetRenderComponents().size();
	for(unsigned int i = 0; i < numRenderComponents; ++i)
	{
		if(renderComponent->GetRenderComponents()[i].componentType == INDEX_BUFFER_RENDER_COMPONENT)
		{
			indexBufferComponentIndex = i;
			break;
		}
	}

	indexBufferComponent = ((IndexBufferComponent*)renderComponent->GetRenderComponents()[indexBufferComponentIndex].component);

	return indexBufferComponent;
}

RenderComponent* BaseObject::LookAtConstantComponent()
{
	ConstantBufferComponent* constantBufferComponent;
	unsigned int constantBufferComponentIndex;

	unsigned int numRenderComponents = renderComponent->GetRenderComponents().size();
	for(unsigned int i = 0; i < numRenderComponents; ++i)
	{
		if(renderComponent->GetRenderComponents()[i].componentType == CONSTANT_BUFFER_RENDER_COMPONENT)
		{
			constantBufferComponentIndex = i;
			break;
		}
	}

	constantBufferComponent = ((ConstantBufferComponent*)renderComponent->GetRenderComponents()[constantBufferComponentIndex].component);

	return constantBufferComponent;
}