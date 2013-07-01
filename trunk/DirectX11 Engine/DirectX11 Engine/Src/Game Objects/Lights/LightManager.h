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

enum LIGHT_TYPE{POINT_LIGHT, SPOT_LIGHT, DIRCTIONAL_LIGHT, AMBIENT_LIGHT};

class LightManager
{
private:
	static AmbientLight* ambientLight;
	static vector<DirectionalLight*> directionalLights;
	static vector<PointLight*> pointLights;

public:
	LightManager();
	LightManager(const LightManager& _lightManager);
	~LightManager();

	static void SetAmbientLight(char* _lightName, XMFLOAT4 _color, bool _isLightOn);
	static void AddDirectionalLight(char* _lightName, XMFLOAT4 _color, XMFLOAT3 _direction, bool _isLightOn);
	static void AddPointLight(char* _lightName, XMFLOAT4 _color, XMFLOAT3 _position, float _radius, bool _isLightOn);

	static void TurnOffAbientLight(){ambientLight->SetIsLightOn(false);}
	static void TurnOnAmbientLight(){ambientLight->SetIsLightOn(true);}
	static void ToggleAmbientLight(){ambientLight->SetIsLightOn(!ambientLight->GetLightOn());}
	static void TurnOffDirectionalLight(int _index, char* _lightName = "");
	static void TurnOnDirectionalLight(int _index, char* _lightName = "");
	static void ToggleDirectionalLight(int _index, char* _lightName = "");
	static void TurnOffPointLight(int _index, char* _lightName = "");
	static void TurnOnPointLight(int _index, char* _lightName = "");
	static void TogglePointLight(int _index, char* _lightName = "");
	
	static int GetNumberPointLights(){return pointLights.size();}
	static int GetNumberDirectionalLights(){return directionalLights.size();}
	static AmbientLight* GetAmbientLight();
	static DirectionalLight* GetDirectionalLight(int _index, char* _lightName = "");
	static PointLight* GetPointLight(int _index, char* _lightName = "");
};
#endif