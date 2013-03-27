#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include <d3d11.h>
#include <d3d11shader.h>
#include "../../Game/Definitions.h"

class DirectionalLight
{
private:
	XMFLOAT4 color;
	XMFLOAT3 direction;
	char* lightName;
	bool isLightOn;

public:
	DirectionalLight() : color(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)), direction(XMFLOAT3(0.0f, 0.0f, 0.0f)), isLightOn (false){}
	DirectionalLight(const DirectionalLight& _directionalLight){;}
	~DirectionalLight(){;}

	XMFLOAT4 GetLightColorF(){return color;}
	XMVECTOR GetLightColorV(){return XMLoadFloat4(&color);}
	XMFLOAT3 GetLightDirectionF(){return direction;}
	XMVECTOR GetLightDirectionV(){return XMLoadFloat3(&direction);}
	char* GetLightName(){return lightName;}
	bool GetLightOn(){return isLightOn;}

	void SetLightColor(XMFLOAT4 _color){color = _color;}
	void SetLightDirection(XMFLOAT3 _direction){direction = _direction;}
	void SetLightName(char* _lightName){lightName = _lightName;}
	void SetIsLightOn(bool _isLightOn){isLightOn = _isLightOn;}
};

#endif // !DIRECTIONALLIGHT_H
