#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include "Camera.h"
#include <vector>
using namespace std;

class CameraManager
{
public:
	CameraManager(){}
	~CameraManager(){}
	CameraManager(const CameraManager&){}

	static void Initialize();
	static void AddCamera(Camera* _camera);
	static void SetCamera(unsigned int _currentCamera);
	static void Shutdown();
	static Camera* GetCurrentCamera(){return *currentCamera;}

	static vector<Camera*> cameras;
	static Camera** currentCamera;
};

#endif