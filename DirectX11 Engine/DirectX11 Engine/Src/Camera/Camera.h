#ifndef CAMERA_H
#define CAMERA_H

#include <string>
using namespace std;
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

enum CAMERA_TYPE{CAMERA_DEFAULT_VIEW, CAMERA_FIRST_PERSON, CAMERA_THIRD_PERSON, CAMERA_FREE_ROAM};

class Camera
{
public:
	Camera();
	Camera(CAMERA_TYPE _cameraType, XMFLOAT4X4 _viewMatrix);
	~Camera();


	/////////////////////////////////////////// NEW CAMERA CODE USING A SINGLE MATRIX///////////////////////////////////////// 	
//  Camera(const XMFLOAT4X4& camMatrix);
// 	XMMATRIX GetPositionMatrixXM() const;
// 	XMMATRIX SetPosition(const XMMATRIX& _m);
// 	
// 	XMMATRIX GetRightMatrixXM() const;
// 	XMMATRIX GetUpMatrixXM() const;
// 	XMMATRIX GetLookMatrixXM() const;
// 
// 	void LookAtMatrix(XMMATRIX _CameraPos, XMMATRIX _targetPos);
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CAMERA_TYPE GetCameraType(){return cameraType;}
	XMVECTOR GetPositionXM() const;
	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetInversePosition() const;

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

	//Get the near and far plane dimensions in-view space coordinates
	float GetNearPlaneWidth() const;
	float GetNearPlaneHeight() const;
	float GetFarPlaneWidth() const;
	float GetFarPlaneHeight() const;

	//Set the frustum
	void SetLens(float _fovY, float _screenWidth, float _screenHeight, float _zNear, float _zFar);

	//Define the camera space via the LookAt parameters
	void LookAt(FXMVECTOR _cameraPos, FXMVECTOR _targetPos, FXMVECTOR _worldUp);
	void LookAt(const XMFLOAT3& _cameraPos, const XMFLOAT3& _targetPos, const XMFLOAT3& _worldUp);


	//Get View/Projection matrices
	XMMATRIX GetViewMatrixM() const;
	XMMATRIX GetProjectionMatrixM() const;
	XMMATRIX GetViewProjectionMatrixM() const;
	XMFLOAT4X4 GetViewMatrixF() const;
	XMFLOAT4X4 GetProjectionMatrixF() const;
	XMFLOAT4X4 GetViewProjectionMatrixF() const;
	void* GetViewMatrixP();
	void* GetInvViewMatrixP();
	void* GetInvViewProjMatrixP();
	void* GetProjectionMatrixP();

	void* GetWidthHeightNearFarP();
	void* GetFrustumExtentsP();

	//Strafe/Walk the camera a distance _d
	void Strafe(float _d);
	void Walk(float _d);
	void Raise(float _d);

	//Rotate the camera an _angle
	void Pitch(float _angleDegrees);
	void Yaw(float _angleDegrees);
	void Roll(float _angleDegrees);

	//After modifying the camera position/orientation, call
	//UpdateViewMatrix() to rebuild the view matrix once per frame
	void UpdateViewMatrix();

private:
	
	CAMERA_TYPE cameraType;
	//Frustum Properties
	float nearZ;
	float farZ;
	float aspectRatio;
	float fovY;
	float nearPlaneHeight;
	float nearPlaneWidth;
	float farPlaneHeight;
	float farPlaneWidth;
	XMFLOAT4 widthHeightNearFar;
	XMFLOAT4 frustumExtents;

	//View/Projection Matrices
	XMFLOAT4X4 view;
	XMFLOAT4X4 inverseView;
	XMFLOAT4X4 inverseViewProjection;
	XMFLOAT4X4 projection;
};
#endif
