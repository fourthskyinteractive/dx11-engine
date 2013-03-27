#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <d3d11.h>
#include <d3d11shader.h>
#include <vector>
using namespace std;

#include "../../Game/Definitions.h"
#include "AmbientLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"

class LightManager
{
private:
	AmbientLight* ambientLight;
	vector<DirectionalLight*> directionalLights;
	vector<PointLight*> pointLights;

public:
	LightManager();
	LightManager(const LightManager& _lightManager);
	~LightManager();

	void SetAmbientLight(char* _lightName, XMFLOAT4 _color, bool _isLightOn);
	void AddDirectionalLight(char* _lightName, XMFLOAT4 _color, XMFLOAT3 _direction, bool _isLightOn);
	void AddPointLight(char* _lightName, XMFLOAT4 _color, XMFLOAT3 _position, float _radius, bool _isLightOn);

	void TurnOffAbientLight(){ambientLight->SetIsLightOn(false);}
	void TurnOnAmbientLight(){ambientLight->SetIsLightOn(true);}
	void ToggleAmbientLight(){ambientLight->SetIsLightOn(!ambientLight->GetLightOn());}
	void TurnOffDirectionalLight(int _index, char* _lightName = "");
	void TurnOnDirectionalLight(int _index, char* _lightName = "");
	void ToggleDirectionalLight(int _index, char* _lightName = "");
	void TurnOffPointLight(int _index, char* _lightName = "");
	void TurnOnPointLight(int _index, char* _lightName = "");
	void TogglePointLight(int _index, char* _lightName = "");
	
	AmbientLight* GetAmbientLight(){return ambientLight;}
	DirectionalLight* GetDirectionalLight(int _index, char* _lightName = "");
	PointLight* GetPointLight(int _index, char* _lightName = "");
};
#endif