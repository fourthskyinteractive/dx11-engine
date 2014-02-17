#include "CameraManager.h"
	
vector<Camera*>		CameraManager::cameras;
Camera**			CameraManager::currentCamera;

void CameraManager::Initialize()
{
	currentCamera = NULL;
}

void CameraManager::AddCamera(Camera* _camera)
{
	cameras.push_back(_camera);

	if(currentCamera == NULL)
	{
		currentCamera = &_camera;
	}
}

void CameraManager::SetCamera(unsigned int _currentCamera)
{
	currentCamera = &cameras[_currentCamera];

	//GO THROUGH OBJECTS AND 
}

void CameraManager::Shutdown()
{
	for(unsigned int i = 0; i < cameras.size(); ++i)
	{
		delete cameras[i];
		cameras[i] = NULL;
	}

	cameras.clear();
}