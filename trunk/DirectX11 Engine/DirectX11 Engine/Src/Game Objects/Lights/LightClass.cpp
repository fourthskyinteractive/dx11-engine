#include "LightClass.h"


LightClass::LightClass()
{
}


LightClass::LightClass(const LightClass& other)
{
}


LightClass::~LightClass()
{
}


void LightClass::SetDiffuseColor(float _red, float _green, float _blue, float _alpha)
{
	diffuseColor = XMFLOAT4(_red, _green, _blue, _alpha);
	return;
}


void LightClass::SetDirection(float x, float y, float z)
{
	direction = XMFLOAT3(x, y, z);
	return;
}

void LightClass::SetAmbientColor(float _red, float _green, float _blue, float _alpha)
{
	ambientColor = XMFLOAT4(_red, _green, _blue, _alpha);
}


XMFLOAT4 LightClass::GetDiffuseColor()
{
	return diffuseColor;
}


XMFLOAT3 LightClass::GetDirection()
{
	return direction;
}

XMFLOAT4 LightClass::GetAmbientColor()
{
	return ambientColor;
}