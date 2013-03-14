#ifndef LIGHTCLASS_H
#define LIGHTCLASS_H

#include <Windows.h>
#include <d3d11.h>
#include <d3d11shader.h>
#include "../../Game/Definitions.h"


class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetAmbientColor(float, float, float, float);

	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT3 GetDirection();

private:
	XMFLOAT4 ambientColor;
	XMFLOAT4 diffuseColor;
	XMFLOAT3 direction;
};

#endif