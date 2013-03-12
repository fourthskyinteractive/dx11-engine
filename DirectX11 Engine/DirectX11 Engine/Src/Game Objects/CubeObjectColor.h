#ifndef CUBEOBJECTCOLOR_H
#define CUBEOBJECTCOLOR_H

#include <Windows.h>
#include <d3d11.h>
#include <d3d11shader.h>
#include "../Game/Definitions.h"
#include "../Renderer/Shader Classes/ColorShader.h"

class CubeObjectColor
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT3 color;
	};

public:
	CubeObjectColor();
	CubeObjectColor(const CubeObjectColor&);
	~CubeObjectColor();

	bool Initialize(ID3D11Device* _device, XMFLOAT3 _pos, XMFLOAT3 _scale, XMFLOAT3 _rotation);
	void Shutdown();
	void Render(ID3D11DeviceContext* _deviceContext);
	void Update(float _dt);
	void UpdateWorldMatrix(XMFLOAT3 _pos, XMFLOAT3 _scale, XMFLOAT3 _rotation);

	int GetIndeCount();

private:
	bool InitializeBuffers(ID3D11Device* _device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* _deviceContext);

private:
	ColorShader shaderUsed;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int vertexCount, indexCount;

	XMFLOAT4X4 worldMatrix;
};

#endif