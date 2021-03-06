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

struct PointLightCompressed
{
	XMFLOAT4 color;
	XMFLOAT3 position;
	float radius;

	PointLightCompressed() : color(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)), position(XMFLOAT3(0.0f, 0.0f, 0.0f)), radius(0.0f){}
};

class LightManager
{
private:
	static AmbientLight* ambientLight;
	static vector<DirectionalLight*> directionalLights;
	static vector<PointLight*> pointLights;
	static vector<PointLightCompressed> pointLightsCompressed;

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
	static void Shutdown();

	static void* GetNumberOfLightsMemory();
	static unsigned int GetNumberPointLights(){return pointLights.size();}
	static unsigned int GetNumberDirectionalLights(){unsigned int poo = directionalLights.size(); return poo;}
	static AmbientLight* GetAmbientLight();
	static DirectionalLight* GetDirectionalLight(int _index, char* _lightName = "");
	static PointLight* GetPointLight(int _index, char* _lightName = "");
	static void* GetPointLightsMemory();
	static void* GetDirectionalLightMemory();

	static XMFLOAT4 numLights;
	static unsigned int numPointLights;
	static unsigned int numDirectionalLights;
};
#endif