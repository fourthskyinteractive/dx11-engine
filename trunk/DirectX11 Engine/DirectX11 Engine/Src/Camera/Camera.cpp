#include "Camera.h"


Camera::Camera()
{
	cameraType = CAMERA_DEFAULT_VIEW;
	//Frustum Properties
	nearZ = 0.0f;
	farZ = 0.0f;
	aspectRatio = 0.0f;
	fovY = 0.0f;
	nearPlaneHeight = 0.0f;
	nearPlaneWidth = 0.0f;
	farPlaneHeight = 0.0f;
	farPlaneWidth = 0.0f;

	//View/Projection Matrices
	XMStoreFloat4x4(&view, XMMatrixIdentity());
	XMStoreFloat4x4(&inverseView, XMMatrixIdentity());
	XMStoreFloat4x4(&projection, XMMatrixIdentity());
}

Camera::Camera(CAMERA_TYPE _cameraType, XMFLOAT4X4 _viewMatrix)
{
	cameraType = _cameraType;
	view = _viewMatrix;
}

Camera::~Camera()
{

}

XMVECTOR Camera::GetPositionXM() const
{
	XMFLOAT3 pos;

	pos.x = view._41;
	pos.y = view._42;
	pos.z = view._43;

	return XMLoadFloat3(&pos);
}

XMFLOAT3 Camera::GetPosition() const
{
	return XMFLOAT3(view._41, view._42, view._43);
}

XMFLOAT3 Camera::GetInversePosition() const
{
	return XMFLOAT3(-view._41, -view._42, -view._43);
}

void Camera::SetPosition(float _x, float _y, float _z)
{
	view._41 = _x;
	view._42 = _y;
	view._43 = _z;	
}

void Camera::SetPosition(const XMFLOAT3& _v)
{
	view._41 = _v.x;
	view._42 = _v.y;
	view._43 = _v.z;
}

void Camera::SetPosition(const XMVECTOR& _v)
{
	XMFLOAT3 pos;

	XMStoreFloat3(&pos,_v);

	view._41 = pos.x;
	view._42 = pos.y;
	view._43 = pos.z;	
}

XMVECTOR Camera::GetRightXM() const
{
	XMFLOAT3 right;

	right.x =  view._11;
	right.y =  view._12;
	right.z =  view._13;

	return XMLoadFloat3(&right);
}

XMFLOAT3 Camera::GetRight() const
{
	XMFLOAT3 _right;

	_right.x =  view._11;
	_right.y =  view._12;
	_right.z =  view._13;
	return _right;
}

XMVECTOR Camera::GetUpXM() const
{
	XMFLOAT3 _up;

	_up.x =  view._21;
	_up.y =  view._22;
	_up.z =  view._23;

	return XMLoadFloat3(&_up);
}

XMFLOAT3 Camera::GetUp() const
{
	XMFLOAT3 _up;

	_up.x =  view._21;
	_up.y =  view._22;
	_up.z =  view._23;

	return _up;
}

XMVECTOR Camera::GetLookXM() const
{
	XMFLOAT3 _look;

	_look.x =  view._31;
	_look.y =  view._32;
	_look.z =  view._33;

	return XMLoadFloat3(&_look);
}

XMFLOAT3 Camera::GetLook() const
{
	XMFLOAT3 _look;

	_look.x =  view._31;
	_look.y =  view._32;
	_look.z =  view._33;

	return _look;
}

float Camera::GetNearZ() const
{
	return nearZ;
}

float Camera::GetFarZ() const
{
	return farZ;
}

float Camera::GetAspect() const
{
	return aspectRatio;
}

float Camera::GetFovY() const
{
	return fovY;
}

float Camera::GetFovX() const
{
	float halfWidth = 0.5f * GetNearPlaneWidth();
	return 2.0f * atan(halfWidth / nearZ);
}

float Camera::GetNearPlaneWidth() const
{
	return nearPlaneWidth;
}

float Camera::GetNearPlaneHeight() const
{
	return nearPlaneHeight;
}

float Camera::GetFarPlaneWidth() const
{
	return farPlaneWidth;
}

float Camera::GetFarPlaneHeight() const
{
	return farPlaneHeight;
}

void Camera::SetLens(float _fovY, float _screenWidth, float _screenHeight, float _zNear, float _zFar)
{
	fovY = _fovY;
	aspectRatio = _screenWidth / _screenHeight;
	nearZ = _zNear;
	farZ = _zFar;
	farPlaneHeight = ((2.0f * tan(XMConvertToRadians(50)) / 2.0f) * farZ);
	farPlaneWidth = (farPlaneHeight * ((float)_screenWidth / (float)_screenHeight));
	nearPlaneHeight = ((2.0f * tan(XMConvertToRadians(50)) / 2.0f) * nearZ);
	nearPlaneWidth = (nearPlaneHeight * ((float)_screenWidth / (float)_screenHeight));

	frustumExtents.x = farPlaneWidth;
	frustumExtents.y = farPlaneHeight;
	frustumExtents.z = nearPlaneWidth;
	frustumExtents.w = nearPlaneHeight;
	widthHeightNearFar.x = _screenWidth;
	widthHeightNearFar.y = _screenHeight;
	widthHeightNearFar.z = _zNear;
	widthHeightNearFar.w = _zFar;
	XMMATRIX proj;

	proj = XMMatrixPerspectiveFovLH(_fovY, aspectRatio, _zNear, _zFar);
	XMStoreFloat4x4(&projection, proj);
}

void Camera::LookAt(FXMVECTOR _cameraPos, FXMVECTOR _targetPos, FXMVECTOR _worldUp)
{
	XMStoreFloat4x4(&view,XMMatrixLookAtLH(_cameraPos, _targetPos, _worldUp) );
}

void Camera::LookAt(const XMFLOAT3& _cameraPos, const XMFLOAT3& _targetPos, const XMFLOAT3& _worldUp)
{
	XMStoreFloat4x4(&view,XMMatrixLookAtLH(XMLoadFloat3(&_cameraPos), XMLoadFloat3(&_targetPos), XMLoadFloat3(&_worldUp)) );
}

XMMATRIX Camera::GetViewMatrixM() const
{
	return XMLoadFloat4x4(&view);
}

XMMATRIX Camera::GetProjectionMatrixM() const
{
	return XMLoadFloat4x4(&projection);
}

XMMATRIX Camera::GetViewProjectionMatrixM() const
{	
	return XMMatrixMultiply(XMMatrixInverse(NULL, XMLoadFloat4x4(&view)), XMLoadFloat4x4(&projection));
}

XMFLOAT4X4 Camera::GetViewMatrixF() const
{
	XMMATRIX viewM;
	viewM = XMLoadFloat4x4(&view);
	viewM = XMMatrixInverse(0, viewM);
	XMFLOAT4X4 tempView;
	XMStoreFloat4x4(&tempView, viewM);
	return tempView;
}

XMFLOAT4X4 Camera::GetProjectionMatrixF() const
{
	return projection;
}

XMFLOAT4X4 Camera::GetViewProjectionMatrixF() const
{
	XMFLOAT4X4 viewProj;
	XMStoreFloat4x4(&viewProj, XMMatrixMultiply(XMMatrixInverse(NULL, XMLoadFloat4x4(&view)), XMLoadFloat4x4(&projection)));
	return viewProj;
}

void* Camera::GetViewMatrixP()
{
	return reinterpret_cast<void*>(&view);
}

void* Camera::GetInvViewMatrixP()
{
	XMMATRIX viewM;
	viewM = XMLoadFloat4x4(&view);
	viewM = XMMatrixInverse(0, viewM);
	XMStoreFloat4x4(&inverseView, viewM);
	return reinterpret_cast<void*>(&inverseView);
}

void* Camera::GetProjectionMatrixP()
{
	return reinterpret_cast<void*>(&projection);
}

void* Camera::GetWidthHeightNearFarP()
{
	return reinterpret_cast<void*>(&widthHeightNearFar);
}

void* Camera::GetFrustumExtentsP()
{
	return reinterpret_cast<void*>(&frustumExtents);
}

void Camera::Strafe(float _d)
{
	XMVECTOR scalar = XMVectorScale(XMVector3Normalize(XMLoadFloat3(&GetRight())), _d);
	
	XMVECTOR pos = XMLoadFloat3(&GetPosition());

	pos += scalar;

	SetPosition(pos);
}

void Camera::Walk(float _d)
{
	XMFLOAT3 l = GetLook();

	XMVECTOR scalar = XMVectorScale(XMVector3Normalize(GetLookXM()), _d);

	XMStoreFloat3(&l, scalar);

	XMVECTOR pos = XMLoadFloat3(&GetPosition());

	XMStoreFloat3(&l, pos);

	pos = XMVectorAdd(pos, scalar);

	XMStoreFloat3(&l, pos);

	SetPosition(pos);
}

void Camera::Raise(float _d)
{
	XMVECTOR scalar = XMVectorScale(XMVector3Normalize(XMLoadFloat3(&GetUp())), _d);


	XMVECTOR pos = XMLoadFloat3(&GetPosition());

	pos += scalar;

	SetPosition(pos);
}

void Camera::Pitch(float _angleDegrees)
{
	XMMATRIX R = XMMatrixRotationX(XMConvertToRadians(_angleDegrees));
	
	XMMATRIX cam;

	cam = XMLoadFloat4x4(&view);

	cam = XMMatrixMultiply(R, cam);

	XMStoreFloat4x4(&view, cam);
}

void Camera::Yaw(float _angleDegrees)
{
	XMMATRIX R = XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMConvertToRadians(_angleDegrees));

	XMMATRIX cam;
	XMFLOAT3 pos = XMFLOAT3(view._41, view._42, view._43);
	view._41 = 0.0f;
	view._42 = 0.0f;
	view._43 = 0.0f;
	view._44 = 1.0f;

    cam = XMLoadFloat4x4(&view);

	cam = XMMatrixMultiply(cam, R);

	XMStoreFloat4x4(&view, cam);
	view._41 = pos.x;
	view._42 = pos.y;
	view._43 = pos.z;
	view._44 = 1.0f;
	UpdateViewMatrix();
}

void Camera::Roll(float _angleDegrees)
{
	XMMATRIX R = XMMatrixRotationZ(XMConvertToRadians(_angleDegrees));
	
	XMMATRIX cam;

	cam = XMLoadFloat4x4(&view);

	cam = XMMatrixMultiply(cam, R);

	XMStoreFloat4x4(&view, cam);

	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	XMMATRIX viewM;
	viewM = XMLoadFloat4x4(&view);
	viewM = XMMatrixInverse(0, viewM);
	XMStoreFloat4x4(&inverseView, viewM);

// 	XMVECTOR lookV = GetLookXM();
// 	XMVECTOR upV = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
// 	lookV = XMVector3Normalize(lookV);
// 	upV = XMVector3Normalize(upV);
// 
// 	XMVECTOR rightV = XMVector3Cross(upV, lookV);
// 
// 	rightV = XMVector3Normalize(rightV);
// 	upV = XMVector3Cross(lookV, rightV);
// 	upV = XMVector3Normalize(upV);
// 
// 	XMFLOAT3 right;
// 	XMFLOAT3 up;
// 	XMFLOAT3 look;
// 	XMFLOAT3 position;
// 
// 	XMStoreFloat3(&right, rightV);
// 	XMStoreFloat3(&look, lookV);
// 	XMStoreFloat3(&up, upV);
// 
// 	XMFLOAT4X4 camera(right.x,			right.y,			right.z, 0.0f,
// 					  up.x,				up.y,				up.z, 0.0f,
// 					  look.x,			look.y,				look.z, 0.0f,
// 					  GetPosition().x,	GetPosition().y,	GetPosition().z, 1.0f);
// 
// 	view = camera;
}