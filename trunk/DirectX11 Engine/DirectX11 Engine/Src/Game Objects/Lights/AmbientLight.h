#ifndef AMBIENTLIGHT_H
#define AMBIENTLIGHT_H

#include <d3d11.h>
#include <d3d11shader.h>
#include "../../Game/Definitions.h"

class AmbientLight
{
private:
	XMFLOAT4 color;
	char* lightName;
	bool isLightOn;

public:
	AmbientLight() : color(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)), isLightOn (false){}
	AmbientLight(const AmbientLight& _directionalLight){};
	~AmbientLight(){};

	XMFLOAT4 GetLightColorF(){return color;}
	XMVECTOR GetLightColorV(){return XMLoadFloat4(&color);}
	char* GetLightName(){return lightName;}
	bool GetLightOn(){return isLightOn;}

	void SetLightColor(XMFLOAT4 _color){color = _color;}
	void SetLightName(char* _lightName){lightName = _lightName;}
	void SetIsLightOn(bool _isLightOn){isLightOn = _isLightOn;}
};

#endif // !DIRECTIONALLIGHT_H
