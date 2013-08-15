#ifndef SKYBOXSHADER_H
#define SKYBOXSHADER_H

#include <d3d11.h>
#include <d3d11shader.h>
#include "../../Game/Definitions.h"
#include "../../Game Objects/Lights/LightManager.h"
#include "BaseShader.h"

class SkyboxShader : public BaseShader
{
private:

	struct SkyboxCBuffer
	{
		XMFLOAT4X4 inverseViewPorjection;
	};

public:
	SkyboxShader();
	SkyboxShader(const SkyboxShader&);
	~SkyboxShader();

	bool Initialize(VERTEX_SHADERS _vertexShader, PIXEL_SHADERS _pixelShader, GEOMETRY_SHADERS _geometryShader);
	void Shutdown();
	bool Render(int _indexCount);
	bool UpdateVertexShaderConstants(XMFLOAT4X4 _worldMatrix, XMFLOAT4X4 _viewProjMatrix);
	bool UpdatePixelShaderTextureConstants(ID3D11ShaderResourceView* _textureArray[]);
	bool UpdatePixelShaderConstants(LIGHT_TYPE _lightType, int lightIndex);
	void UpdatePerFrame(ID3D11RenderTargetView* _renderTarget, ID3D11ShaderResourceView* _texture[] = NULL);
	void UpdatePerLight(LIGHT_TYPE _lightType, int _lightIndex);

private:
	bool InitializeShader(int _vertexShaderIndex, int  _pixelShaderIndex, int _geometryShaderIndex);
	void ShutdownShader();
	void RenderShader(int _indexCount);
	void SetShader();

private:
	unsigned int vertexShaderIndex;
	unsigned int pixelShaderIndex;
	unsigned int geometryShaderIndex;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* constantBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* geometryConstantBuffer;
};

#endif
