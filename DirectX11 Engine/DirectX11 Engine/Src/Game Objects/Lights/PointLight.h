#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <d3d11.h>
#include <d3d11shader.h>
#include "../../Game/Definitions.h"

class PointLight
{
private:
	XMFLOAT4 color;
	XMFLOAT3 position;
	char* lightName;
	float radius;
	bool isLightOn;

public:
	PointLight() : color(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)), position(XMFLOAT3(0.0f, 0.0f, 0.0f)), isLightOn (false){}
	PointLight(const PointLight& _directionalLight){};
	~PointLight(){};

	XMFLOAT4 GetLightColorF(){return color;}
	XMVECTOR GetLightColorV(){return XMLoadFloat4(&color);}
	XMFLOAT3 GetLightPositionF(){return position;}
	XMVECTOR GetLightPositionV(){return XMLoadFloat3(&position);}
	char* GetLightName(){return lightName;}
	float GetLightRadius(){return radius;}
	bool GetLightOn(){return isLightOn;}

	void SetLightColor(XMFLOAT4 _color){color = _color;}
	void SetLightPosition(XMFLOAT3 _position){position = _position;}
	void SetLightName(char* _lightName){lightName = _lightName;}
	void SetLightRadius(float _radius){radius = _radius;}
	void SetIsLightOn(bool _isLightOn){isLightOn = _isLightOn;}
};

#endif // !DIRECTIONALLIGHT_H
