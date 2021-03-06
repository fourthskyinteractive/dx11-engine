#include "LightManager.h"

AmbientLight*					LightManager::ambientLight;
vector<DirectionalLight*>		LightManager::directionalLights;
vector<PointLight*>				LightManager::pointLights;
vector<PointLightCompressed>	LightManager::pointLightsCompressed;

unsigned int				LightManager::numDirectionalLights;
unsigned int				LightManager::numPointLights;
XMFLOAT4					LightManager::numLights;

LightManager::LightManager()
{
	numDirectionalLights = 0;
	numPointLights = 0;
	ZeroMemory(&numLights, sizeof(XMFLOAT4));
}

LightManager::LightManager(const LightManager& _lightManager)
{

}

LightManager::~LightManager()
{
	
}

void LightManager::Shutdown()
{
	//Cleaning up Point Lights
	for(unsigned int i = 0; i < pointLights.size(); ++i)
	{
		SAFE_DELETE(pointLights[i]);
	}

	//Cleaning up Directional Lights
	for(unsigned int i = 0; i < directionalLights.size(); ++i)
	{
		SAFE_DELETE(directionalLights[i]);
	}

	//Cleaning up the Ambient Light
	SAFE_DELETE(ambientLight);
}

void LightManager::SetAmbientLight(char* _lightName, XMFLOAT4 _color, bool _isLightOn)
{
	AmbientLight* aLight = new AmbientLight();

	aLight->SetLightName(_lightName);
	aLight->SetLightColor(_color);
	aLight->SetIsLightOn(_isLightOn);

	ambientLight = aLight;
}

void LightManager::AddDirectionalLight(char* _lightName, XMFLOAT4 _color, XMFLOAT3 _direction, bool _isLightOn)
{
	DirectionalLight* directionalLight = new DirectionalLight();

	directionalLight->SetLightName(_lightName);
	directionalLight->SetLightColor(_color);
	directionalLight->SetLightDirection(_direction);
	directionalLight->SetIsLightOn(_isLightOn);

	directionalLights.push_back(directionalLight);

	numDirectionalLights ++;
	numLights.x += 1.0f;
}

void LightManager::AddPointLight(char* _lightName, XMFLOAT4 _color, XMFLOAT3 _position, float _radius, bool _isLightOn)
{
	PointLight* pointLight = new PointLight();

	pointLight->SetLightName(_lightName);
	pointLight->SetLightColor(_color);
	pointLight->SetLightPosition(_position);
	pointLight->SetLightRadius(_radius);
	pointLight->SetIsLightOn(_isLightOn);

	pointLights.push_back(pointLight);

	numPointLights ++;
	numLights.y += 1.0f;

	//ADD Compressed Point Light
	PointLightCompressed pointLightCompressed;// = new PointLightCompressed();
	pointLightCompressed.color = _color;
	pointLightCompressed.position = _position;
	pointLightCompressed.radius = _radius;
	pointLightsCompressed.push_back(pointLightCompressed);
}


//TODO:: GO THROUGH AND MAKE SURE THAT ALL OF THIS IS GOOD!
// void LightManager::TurnOffDirectionalLight(int _index, char* _lightName /* = "" */)
// {
// 	int numDirectionalLights = directionalLights.size();
// 
// 	if(_lightName != "")
// 	{
// 		for(int i = 0; i < numDirectionalLights; ++i)
// 		{
// 			if(directionalLights[i]->GetLightName() == _lightName)
// 			{
// 				directionalLights[i]->SetIsLightOn(false);
// 			}
// 		}
// 	}
// 
// 	directionalLights[_index]->SetIsLightOn(false);
// 
// 	numDirectionalLights --;
// }
// 
// void LightManager::TurnOnDirectionalLight(int _index, char* _lightName /* = "" */)
// {
// 	int numDirectionalLights = directionalLights.size();
// 	if(_lightName != "")
// 	{
// 		for(int i = 0; i < numDirectionalLights; ++i)
// 		{
// 			if(directionalLights[i]->GetLightName() == _lightName)
// 			{
// 				directionalLights[i]->SetIsLightOn(true);
// 			}
// 		}
// 	}
// 	
// 	directionalLights[_index]->SetIsLightOn(true);
// 
// 	numDirectionalLights ++;
// }
// 
// void LightManager::ToggleDirectionalLight(int _index, char* _lightName /* = "" */)
// {
// 	int numDirectionalLights = directionalLights.size();
// 	if(_lightName != "")
// 	{
// 		for(int i = 0; i < numDirectionalLights; ++i)
// 		{
// 			if(directionalLights[i]->GetLightName() == _lightName)
// 			{
// 				directionalLights[i]->SetIsLightOn(!directionalLights[i]->GetLightOn());
// 			}
// 		}
// 	}
// 
// 	directionalLights[_index]->SetIsLightOn(!directionalLights[_index]->GetLightOn());
// 
// 	
// }
// 
// void LightManager::TurnOffPointLight(int _index, char* _lightName /* = "" */)
// {
// 	int numPointLights = pointLights.size();
// 	if(_lightName != "")
// 	{
// 		for(int i = 0; i < numPointLights; ++i)
// 		{
// 			if(pointLights[i]->GetLightName() == _lightName)
// 			{
// 				pointLights[i]->SetIsLightOn(false);
// 			}
// 		}
// 	}
// 
// 	pointLights[_index]->SetIsLightOn(false);
// }
// 
// void LightManager::TurnOnPointLight(int _index, char* _lightName /* = "" */)
// {
// 	int numPointLights = pointLights.size();
// 	if(_lightName != "")
// 	{
// 		for(int i = 0; i < numPointLights; ++i)
// 		{
// 			if(pointLights[i]->GetLightName() == _lightName)
// 			{
// 				pointLights[i]->SetIsLightOn(true);
// 			}
// 		}
// 	}
// 
// 	pointLights[_index]->SetIsLightOn(true);
// }
// 
// void LightManager::TogglePointLight(int _index, char* _lightName /* = "" */)
// {
// 	int numPointLights = pointLights.size();
// 	if(_lightName != "")
// 	{
// 		for(int i = 0; i < numPointLights; ++i)
// 		{
// 			if(pointLights[i]->GetLightName() == _lightName)
// 			{
// 				pointLights[i]->SetIsLightOn(!pointLights[i]->GetLightOn());
// 			}
// 		}
// 	}
// 
// 	pointLights[_index]->SetIsLightOn(!pointLights[_index]->GetLightOn());
// }

AmbientLight* LightManager::GetAmbientLight()
{
	if(ambientLight)
	{
		return ambientLight;
	}

	return NULL;
}

DirectionalLight* LightManager::GetDirectionalLight(int _index, char* _lightName /* = "" */)
{
	int numDirectionalLights = directionalLights.size();
	if(_lightName != "")
	{
		for(int i = 0; i < numDirectionalLights; ++i)
		{
			if(directionalLights[i]->GetLightName() == _lightName)
			{
				return directionalLights[i];
			}
		}
	}

	return directionalLights[_index];
}

PointLight* LightManager::GetPointLight(int _index, char* _lightName /* = "" */)
{
	int numPointLights = pointLights.size();
	if(_lightName != "")
	{
		for(int i = 0; i < numPointLights; ++i)
		{
			if(pointLights[i]->GetLightName() == _lightName)
			{
				return pointLights[i];
			}
		}
	}

	return pointLights[_index];
}

void* LightManager::GetNumberOfLightsMemory()
{
	return &numLights;
}

void* LightManager::GetPointLightsMemory()
{
	if(numPointLights > 0)
		return &pointLightsCompressed[0];
	else
		return NULL;
}

void* LightManager::GetDirectionalLightMemory()
{
	if(numDirectionalLights > 0)
		return &directionalLights[0];
	else
		return NULL;
}