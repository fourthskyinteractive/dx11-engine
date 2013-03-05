#include "Camera.h"


Camera::Camera(const XMFLOAT3& _position, const XMFLOAT3& _look, const XMFLOAT3& _up, const XMFLOAT3& _right)
{
	position = _position;
	look = _look;
	up = _up;
	right = _right;
}

Camera::~Camera()
{

}

XMVECTOR Camera::GetPositionXM() const
{
	return XMLoadFloat3(&position);
}

XMFLOAT3 Camera::GetPosition() const
{
	return position;
}

void Camera::SetPosition(float _x, float _y, float _z)
{
	position.x = _x;
	position.y = _y;
	position.z = _z;
}

void Camera::SetPosition(const XMFLOAT3& _v)
{
	position = _v;
}

void Camera::SetPosition(const XMVECTOR& _v)
{
	XMStoreFloat3(&position, _v);
}

XMVECTOR Camera::GetRightXM() const
{
	return XMLoadFloat3(&right);
}

XMFLOAT3 Camera::GetRight() const
{
	return right;
}

XMVECTOR Camera::GetUpXM() const
{
	return XMLoadFloat3(&up);
}

XMFLOAT3 Camera::GetUp() const
{
	return up;
}

XMVECTOR Camera::GetLookXM() const
{
	return XMLoadFloat3(&look);
}

XMFLOAT3 Camera::GetLook() const
{
	return look;
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
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / nearZ);
}

float Camera::GetNearWindowWidth() const
{
	return aspectRatio * nearWindowHeight;
}

float Camera::GetNearWindowHeight() const
{
	return nearWindowHeight;
}

float Camera::GetFarWindowWidth() const
{
	return aspectRatio * farWindowHeight;
}

float Camera::GetFarWindowHeight() const
{
	return farWindowHeight;
}

void Camera::SetLens(float _fovY, float _aspectRatio, float _zNear, float _zFar)
{
	fovY = _fovY;
	aspectRatio = _aspectRatio;
	nearZ = _zNear;
	farZ = _zFar;

	XMMATRIX proj;

	proj = XMMatrixPerspectiveFovLH(_fovY, _aspectRatio, _zNear, _zFar);
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

XMMATRIX Camera::GetViewMatrix() const
{
	return XMLoadFloat4x4(&view);
}

XMMATRIX Camera::GetProjectionMatrix() const
{
	return XMLoadFloat4x4(&projection);
}

XMMATRIX  Camera::GetViewProjectionMatrix() const
{	
	return XMMatrixMultiply(XMLoadFloat4x4(&view), XMLoadFloat4x4(&projection));
}

void Camera::Strafe(float _d)
{
	//position += d * look
	XMVECTOR s = XMVectorReplicate(_d);
	XMVECTOR r = XMLoadFloat3(&right);
	XMVECTOR p = XMLoadFloat3(&position);
	XMStoreFloat3(&position, XMVectorMultiplyAdd(s, r, p));
}

void Camera::Walk(float _d)
{
	XMVECTOR s = XMVectorReplicate(_d);
	XMVECTOR l = XMLoadFloat3(&look);
	XMVECTOR p = XMLoadFloat3(&position);
	XMStoreFloat3(&position, XMVectorMultiplyAdd(s, l, p));
}

void Camera::Pitch(float _angleDegrees)
{
	//Rotate up and look vector about the right vector
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&right), XMConvertToRadians(_angleDegrees));

	XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), R));
	XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), R));
}

void Camera::Yaw(float _angleDegrees)
{
	XMMATRIX R = XMMatrixRotationY(XMConvertToRadians(_angleDegrees));

	XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), R));
	XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), R));
	XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), R));
}

void Camera::Roll(float _angleDegrees)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&look), XMConvertToRadians(_angleDegrees));

	XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), R));
	XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), R));
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR R = XMLoadFloat3(&right);
	XMVECTOR U = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR L = XMLoadFloat3(&look);
	XMVECTOR P = XMLoadFloat3(&position);

	//Make sure that the right, up, and look vectors are all mutually orthogonal
	
	//Make the look unit length
	L = XMVector3Normalize(L);

	//Compute a new corrected "up" vector and normalize it
	R = XMVector3Normalize(XMVector3Cross(L, U));

	//Compute a new corrected "Up" vector, U and L are already normalized so there
	//is no reason to normalize the cross product
	//U = XMVector3Cross(R, L);

	XMMATRIX mView = XMMatrixLookAtLH(P, L + P, U);

	XMStoreFloat4x4(&view, mView);
}