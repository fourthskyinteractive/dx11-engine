#ifndef CAMERA_H
#define CAMERA_H

#include <windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;


class Camera
{
public:
	Camera();
	~Camera();

	XMVECTOR GetPositionXM() const;
	XMFLOAT3 GetPosition() const;

	void SetPosition(float _x, float _y, float _z);
	void SetPosition(const XMFLOAT3& _v);
	void SetPosition(const XMVECTOR& _v);

	//Get the camera basic vectors
	XMVECTOR GetRightXM() const;
	XMFLOAT3 GetRight() const;
	XMVECTOR GetUpXM() const;
	XMFLOAT3 GetUp() const;
	XMVECTOR GetLookXM() const;
	XMFLOAT3 GetLook() const;

	//Get the frustum properties
	float GetNearZ() const;
	float GetFarZ() const;
	float GetAspect() const;
	float GetFovY() const;	
	float GetFovX() const;

	//Get the near and far plane dimensions inview space coordinates
	float GetNearWindowWidth() const;
	float GetNearWindowHeight() const;
	float GetFarWindowWidth() const;
	float GetFarWindowHeight() const;

	//Set the frustum
	void SetLens(float _fovY, float _aspectRatio, float _zNear, float _zFar);

	//Define the camera space via the LookAt parameters
	void LookAt(FXMVECTOR _cameraPos, FXMVECTOR _targetPos, FXMVECTOR _worldUp);
	void LookAt(const XMFLOAT3& _cameraPos, const XMFLOAT3& _targetPos, const XMFLOAT3& _worldUp);

	//Get View/Projection matrices
	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjectionMatrix() const;
	XMMATRIX GetViewProjectionMatrix() const;

	//Strafe/Walk the camera a distance _d
	void Strafe(float _d);
	void Walk(float _d);

	//Rotate the camera an _angle
	void Pitch(float _angleDegrees);
	void Yaw(float _angleDegrees);
	void Roll(float _angleDegrees);

	//After modifying the camera position/orientation, call
	//UpdateViewMatrix() to rebuild the view matrix once per frame
	void UpdateViewMatrix();

private:
	
	//Camera coordinate system with coordinates relative to world space
	XMFLOAT3 position;
	XMFLOAT3 right;
	XMFLOAT3 up;
	XMFLOAT3 look;

	//Frustum Properties
	float nearZ;
	float farZ;
	float aspectRatio;
	float fovY;
	float nearWindowHeight;
	float farWindowHeight;

	//View/Projection Matrices
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
};
#endif
