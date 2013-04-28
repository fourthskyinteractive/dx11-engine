#ifndef LIGHTOBJECTS_H
#define LIGHTOBJECTS_H

#include <d3d11.h>
#include <d3d11shader.h>

#include "../Game/Definitions.h"
#include "../Renderer/TextureManager.h"
#include "../Renderer/Shader Classes/LightShader.h"
#include "../Renderer/Shader Classes/DepthShader.h"
#include "../Renderer/Shader Classes/DeferredShader.h"

class ParentMeshObject;
class DepthShader;

class LightObjects
{
private:
	static ID3D11Buffer* vertexBuffer;
	static ID3D11Buffer* indexbuffer;

	static ID3D11ShaderResourceView* texture;
	static ID3D11RenderTargetView* renderTarget;

	//DeferredShader shaderUsed;

public:
	LightObjects();
	LightObjects(const LightObjects& _childMeshObject);
	~LightObjects();

	static void Initialize(ID3D11RenderTargetView* _renderTargetView, ID3D11ShaderResourceView* _shaderResourceView);
	static void SetShaderBuffers();
	static void ChangeShaderResourceView(ID3D11ShaderResourceView* _shaderResourceView);
	static void Render();
	static void Update();

	//D3DX Buffer Initialization
	static bool InitializeBuffers(VertexType* _vertices, unsigned long* _indices);
};
#endif
