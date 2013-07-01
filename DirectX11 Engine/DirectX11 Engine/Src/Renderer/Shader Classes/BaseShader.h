#ifndef BASESHADER_H
#define BASESHADER_H

#include <d3d11.h>
#include <d3d11shader.h>
#include "../../Game/Definitions.h"
#include "../ShaderManager.h"
//#include "../../Game Objects/ChildMeshObject.h"

enum SHADER_TO_USE{BASE_SHADER, DIFFUSE_SHADER, DEPTH_SHADER, NORMAL_SHADER, LIGHT_SHADER};

class ChildMeshObject;

class BaseShader
{
public:
	BaseShader(){shaderToUse = BASE_SHADER;}
	virtual ~BaseShader(){;}

	virtual bool Initialize(VERTEX_SHADERS _vertexShader, PIXEL_SHADERS _pixelShader, GEOMETRY_SHADERS _geometryShader){return true;}
	virtual void Shutdown(){;}
	virtual bool Render(int _indexCount, ID3D11RenderTargetView* _renderTarget){return true;}
	virtual void Update(ChildMeshObject* _obj, ID3D11ShaderResourceView* _texture = NULL){;}
	virtual void SetShader(){;}
	void SetShaderToUse(int shaderToUse){shaderToUse = shaderToUse;}
	int GetBufferType(){return shaderToUse;}

private:
	virtual bool InitializeShader(int _vertexShaderIndex, int  _pixelShaderIndex, int geometryShaderIndex = 0){return true;}
	virtual void ShutdownShader(){;}
	virtual void RenderShader(int _indexCount, ID3D11RenderTargetView* _renderTarget){;}

	SHADER_TO_USE shaderToUse;
};
#endif
