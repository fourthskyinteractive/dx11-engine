#ifndef LIGHTSHADER_H
#define LIGHTSHADER_H

#include <d3d11.h>
#include <d3d11shader.h>
#include "../../Game/Definitions.h"
#include "BaseShader.h"

class LightShader : public BaseShader
{
private:

	struct MatrixBufferType
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 viewProjection;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;	//Added extra padding so the structure is a multiple
						//of 16 for CreateBuffer function requirements
	};

public:
	LightShader();
	LightShader(const LightShader&);
	~LightShader();

	bool Initialize();
	void Shutdown();
	bool Render(int _indexCount, ID3D11RenderTargetView* _renderTarget);
	bool UpdatePixelShaderTextureConstants(ID3D11ShaderResourceView* _textureArray);
	bool UpdatePixelShaderLightConstants(XMFLOAT3 _lightDirection, XMFLOAT4 _diffuseColor, XMFLOAT4 _ambientColor);
	bool UpdateVertexShaderConstants(XMFLOAT4X4 _worldMatrix, XMFLOAT4X4 _viewProjMatrix);
	void Update(ChildMeshObject* _obj, ID3D11ShaderResourceView* _texture = NULL);
	void SetShader();

private:
	bool InitializeShader(int _vertexShaderIndex, int  _pixelShaderIndex);
	void ShutdownShader();
	void RenderShader(int _indexCount, ID3D11RenderTargetView* _renderTarget);

private:
	unsigned int vertexShaderIndex;
	unsigned int pixelShaderIndex;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* constantBuffer;
	ID3D11SamplerState* sampleState;

	ID3D11Buffer* lightBuffer;
};

#endif
