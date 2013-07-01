#ifndef DEFERREDSHADER_H
#define DEFERREDSHADER_H

#include <d3d11.h>
#include <d3d11shader.h>
#include "../../Game/Definitions.h"
#include "../../Game Objects/Lights/LightManager.h"
#include "BaseShader.h"

class DeferredShader : public BaseShader
{
private:

	struct MatrixBufferType
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 viewProjection;
	};

	struct CombinePixelShaderBufferType
	{
		XMFLOAT4 __declspec(align(16)) lightType;
		XMFLOAT3 __declspec(align(16)) lightPos;
		XMFLOAT3 __declspec(align(16)) lightColor;
		XMFLOAT3 __declspec(align(16)) lightDirection;
		XMFLOAT2 __declspec(align(16)) spotlightAngles;
		XMFLOAT4 __declspec(align(16)) lightRange;
		XMFLOAT3 __declspec(align(16)) cameraPos;
	};

public:
	DeferredShader();
	DeferredShader(const DeferredShader&);
	~DeferredShader();

	bool Initialize(VERTEX_SHADERS _vertexShader, PIXEL_SHADERS _pixelShader, GEOMETRY_SHADERS _geometryShader);
	void Shutdown();
	bool Render(int _indexCount, ID3D11RenderTargetView* _renderTarget);
	bool UpdateVertexShaderConstants(XMFLOAT4X4 _worldMatrix, XMFLOAT4X4 _viewProjMatrix);
	bool UpdatePixelShaderTextureConstants(ID3D11ShaderResourceView* _textureArray[]);
	bool UpdatePixelShaderConstants(LIGHT_TYPE _lightType, int lightIndex);
	void Update(ChildMeshObject* _obj, ID3D11ShaderResourceView* _texture[] = NULL);
	void Update(LIGHT_TYPE _lightType, int _lightIndex, ChildMeshObject* _obj, ID3D11ShaderResourceView* _texture[] = NULL);

private:
	bool InitializeShader(int _vertexShaderIndex, int  _pixelShaderIndex, int _geometryShaderIndex);
	void ShutdownShader();
	void RenderShader(int _indexCount, ID3D11RenderTargetView* _renderTarget);
	void SetShader();

private:
	unsigned int vertexShaderIndex;
	unsigned int pixelShaderIndex;
	unsigned int geometryShaderIndex;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* constantBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* pixelConstantBuffer;
};

#endif
