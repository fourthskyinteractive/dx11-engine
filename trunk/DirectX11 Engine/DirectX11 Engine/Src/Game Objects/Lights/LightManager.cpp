#include "LightManager.h"

LightManager::LightManager()
{

}

LightManager::LightManager(const LightManager& _lightManager)
{

}

LightManager::~LightManager()
{

}

void LightManager::SetAmbientLight(char* _lightName, XMFLOAT4 _color, bool _isLightOn)
{
	AmbientLight* ambientLight = new AmbientLight();

	ambientLight->SetLightName(_lightName);
	ambientLight->SetLightColor(_color);
	ambientLight->SetIsLightOn(_isLightOn);

	this->ambientLight = ambientLight;
}

void LightManager::AddDirectionalLight(char* _lightName, XMFLOAT4 _color, XMFLOAT3 _direction, bool _isLightOn)
{
	DirectionalLight* directionalLight = new DirectionalLight();

	directionalLight->SetLightName(_lightName);
	directionalLight->SetLightColor(_color);
	directionalLight->SetLightDirection(_direction);
	directionalLight->SetIsLightOn(_isLightOn);

	directionalLights.push_back(directionalLight);
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
}

void LightManager::TurnOffDirectionalLight(int _index, char* _lightName /* = "" */)
{
	int numDirectionalLights = directionalLights.size();
	if(_lightName != "")
	{
		for(int i = 0; i < numDirectionalLights; ++i)
		{
			if(directionalLights[i]->GetLightName() == _lightName)
			{
				directionalLights[i]->SetIsLightOn(false);
			}
		}
	}

	directionalLights[_index]->SetIsLightOn(false);
}

void LightManager::TurnOnDirectionalLight(int _index, char* _lightName /* = "" */)
{
	int numDirectionalLights = directionalLights.size();
	if(_lightName != "")
	{
		for(int i = 0; i < numDirectionalLights; ++i)
		{
			if(directionalLights[i]->GetLightName() == _lightName)
			{
				directionalLights[i]->SetIsLightOn(true);
			}
		}
	}
	
	directionalLights[_index]->SetIsLightOn(true);
}

void LightManager::ToggleDirectionalLight(int _index, char* _lightName /* = "" */)
{
	int numDirectionalLights = directionalLights.size();
	if(_lightName != "")
	{
		for(int i = 0; i < numDirectionalLights; ++i)
		{
			if(directionalLights[i]->GetLightName() == _lightName)
			{
				directionalLights[i]->SetIsLightOn(!directionalLights[i]->GetLightOn());
			}
		}
	}

	directionalLights[_index]->SetIsLightOn(!directionalLights[_index]->GetLightOn());
}

void LightManager::TurnOffPointLight(int _index, char* _lightName /* = "" */)
{
	int numPointLights = pointLights.size();
	if(_lightName != "")
	{
		for(int i = 0; i < numPointLights; ++i)
		{
			if(pointLights[i]->GetLightName() == _lightName)
			{
				pointLights[i]->SetIsLightOn(false);
			}
		}
	}

	pointLights[_index]->SetIsLightOn(false);
}

void LightManager::TurnOnPointLight(int _index, char* _lightName /* = "" */)
{
	int numPointLights = pointLights.size();
	if(_lightName != "")
	{
		for(int i = 0; i < numPointLights; ++i)
		{
			if(pointLights[i]->GetLightName() == _lightName)
			{
				pointLights[i]->SetIsLightOn(true);
			}
		}
	}

	pointLights[_index]->SetIsLightOn(true);
}

void LightManager::TogglePointLight(int _index, char* _lightName /* = "" */)
{
	int numPointLights = pointLights.size();
	if(_lightName != "")
	{
		for(int i = 0; i < numPointLights; ++i)
		{
			if(pointLights[i]->GetLightName() == _lightName)
			{
				pointLights[i]->SetIsLightOn(!pointLights[i]->GetLightOn());
			}
		}
	}

	pointLights[_index]->SetIsLightOn(!pointLights[_index]->GetLightOn());
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