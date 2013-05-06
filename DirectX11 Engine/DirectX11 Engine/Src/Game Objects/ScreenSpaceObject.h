#ifndef SCREENSPACEOBJECT_H
#define SCREENSPACEOBJECT_H

#include <d3d11.h>
#include <d3d11shader.h>

#include "../Game/Definitions.h"
#include "../Renderer/TextureManager.h"
#include "../Renderer/Shader Classes/LightShader.h"
#include "../Renderer/Shader Classes/DepthShader.h"
#include "../Renderer/Shader Classes/DeferredShader.h"

class ParentMeshObject;
class DepthShader;

class ScreenSpaceObject
{
private:
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexbuffer;

	ID3D11ShaderResourceView* textures[4];
	ID3D11RenderTargetView* renderTarget;
	ParentMeshObject* parent;

	DeferredShader shaderUsed;

public:
	ScreenSpaceObject();
	ScreenSpaceObject(const ScreenSpaceObject& _screenSpaceObject);
	~ScreenSpaceObject();

	void Initialize(ID3D11RenderTargetView* _renderTargetView, ID3D11ShaderResourceView* _shaderResourceView);
	void SetShaderBuffers();
	void ChangeShaderResourceView(ID3D11ShaderResourceView* _shaderResourceView[]);
	void Render();
	void Update(LIGHT_TYPE _lightType, int _lightIndex);

	//D3DX Buffer Initialization
	bool InitializeBuffers(VertexType* _vertices, unsigned long* _indices);
};
#endif
