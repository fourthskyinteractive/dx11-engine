#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include "../Game Objects/Component System/Component.h"
#include "../Game Objects/Component System/BaseComponent.h"
#include "../Game Objects/Component System/RenderComponent.h"
#include "../Game Objects/Component System/VertexBufferComponent.h"
#include "../Game Objects/Component System/IndexBufferComponent.h"
#include "../Game Objects/Component System/ConstantBufferComponent.h"

#include "../Game/Definitions.h"
#include <atlbase.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <dxgi.h>
using namespace DirectX;

#include <d3d11.h>
#include <d3d11shader.h>
#include <vector>
using namespace std;


class BaseObject
{
public:
	BaseObject();
	BaseObject(const BaseObject&);
	~BaseObject();

	void AddBaseComponent(BASE_COMPONENTS _componentType);
	void AddRenderComponent(RENDER_COMPONENTS _componentType);
	void AddVertexBufferComponent(VERTEX_BUFFER_COMPONENTS _componentType, void* _data, unsigned int _stride, unsigned int _totalSize);
	void AddIndexBufferComponent(INDEX_BUFFER_COMPONENTS _componentType, void* _data, unsigned int _stride, unsigned int _totalSize);
	void AddConstantBufferComponent(CONSTANT_BUFFER_COMPONENTS _componentType, void* _data, unsigned int _totalSize, void* _memoryAddress);
	void AddComputeShaderBuffer(void* _data, unsigned int _stride, unsigned int _totalSize, COMPUTEBUFFERTYPE _bufferType = COMPUTE_NO_TYPE_BUFFER);
	void AddComputeShaderBuffer(CComPtr<ID3D11Buffer> _buffer, unsigned int _stride = 0, unsigned int _totalSize = 0);
// 	void AddComputeShaderBuffer(CComPtr<ID3D11ShaderResourceView> _srv, unsigned int _stride = 0, unsigned int _totalSize = 0);
// 	void AddComputeShaderBuffer(CComPtr<ID3D11UnorderedAccessView> _uav, unsigned int _stride = 0, unsigned int _totalSize = 0);

	void AddTexture(WCHAR* _filePath);
	void AddTexture(CComPtr<ID3D11ShaderResourceView> _shaderResourceView);

	virtual void LoadModel(char* _filePath, ModelData& _modelData);
	virtual void UpdateShaderConstantBuffers();
	virtual void BindRenderComponents();
	virtual void SetShaders(int _vertexShader, int _geometryShader, int _pixelShader, int _computeShader);
	virtual void FinalizeObject();
	virtual void Render();
	virtual void Update(float _dt);

	virtual void Destroy();

	RenderComponent* GetRenderComponent();
	VertexBufferComponent* GetVertexBufferComponent();
	IndexBufferComponent* GetIndexBufferComponent();
	ConstantBufferComponent* GetConstantBufferComponent();
	bool GetRenderable(){return renderable;}
	CComPtr<ID3D11VertexShader> GetVertexShader(){return renderDataMembers->vertexShader;}
	CComPtr<ID3D11PixelShader> GetPixelShader(){return renderDataMembers->pixelShader;}
	CComPtr<ID3D11GeometryShader> GetGeometryShader(){return renderDataMembers->geometryShader;}
	CComPtr<ID3D11ComputeShader> GetComputeShader(){return renderDataMembers->computeShader;}
	DX11RenderDataMembers* GetRenderDataMembers(){return renderDataMembers;}
	vector<unsigned int> GetTextureIndices(){return textureIndices;}
	unsigned int GetNumberOfIndices(){return numIndices;}
	BuffersForBinding* GetBuffersForBinding(){return vertexBufferInformationForBinding;}

	void SetRenderable(bool _renderable){renderable = _renderable;}
	void SetInputLayout(CComPtr<ID3D11InputLayout> _inputLayout){renderDataMembers->inputLayout = _inputLayout;}


	bool IsRenderable(){return renderable;}
	RenderComponent* LookAtVertexComponent();
	RenderComponent* LookAtIndexComponent();
	RenderComponent* LookAtConstantComponent();

private:

	BaseComponent* baseComponent;
	DX11RenderDataMembers* renderDataMembers;
	BuffersForBinding* vertexBufferInformationForBinding;


	RenderComponent* renderComponent;
	VertexBufferComponent* vertexComponent;
	IndexBufferComponent* indexComponent;
	ConstantBufferComponent* constantComponent;

	bool renderable;
	bool useComputeShader;
	unsigned int numVertexComponents;
	unsigned int numIndices;

	vector<unsigned int> textureIndices;
};

#endif