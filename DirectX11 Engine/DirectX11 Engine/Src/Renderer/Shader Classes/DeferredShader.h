#ifndef DEFERREDSHADER_H
#define DEFERREDSHADER_H

#include <d3d11.h>
#include <d3d11shader.h>
#include "../../Game/Definitions.h"
#include "BaseShader.h"

class DeferredShader : public BaseShader
{
private:

	struct MatrixBufferType
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 viewProjection;
	};

public:
	DeferredShader();
	DeferredShader(const DeferredShader&);
	~DeferredShader();

	bool Initialize();
	void Shutdown();
	bool Render(int _indexCount, ID3D11RenderTargetView* _renderTarget);
	bool UpdateVertexShaderConstants(XMFLOAT4X4 _worldMatrix, XMFLOAT4X4 _viewProjMatrix);
	bool UpdatePixelShaderTextureConstants(ID3D11ShaderResourceView* _textureArray[]);
	void Update(ChildMeshObject* _obj, ID3D11ShaderResourceView* _texture[] = NULL);

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
};

#endif
