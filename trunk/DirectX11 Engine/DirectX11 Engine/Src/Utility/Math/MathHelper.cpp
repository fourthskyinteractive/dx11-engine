////***************************************************************************************
//// MathHelper.cpp by Frank Luna (C) 2011 All Rights Reserved.
////***************************************************************************************
//
//#include "MathHelper.h"
//#include <float.h>
//#include <cmath>
//
//const float MathHelper::Infinity = FLT_MAX;
//const float MathHelper::Pi       = 3.1415926535f;
//const float MathHelper::M_E      = 2.71828182845904523536f;
//
//float MathHelper::AngleFromXY(float x, float y)
//{
//	float theta = 0.0f;
// 
//	// Quadrant I or IV
//	if(x >= 0.0f) 
//	{
//		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
//		//                atanf(y/x) = -pi/2 if y < 0
//		theta = atanf(y / x); // in [-pi/2, +pi/2]
//
//		if(theta < 0.0f)
//			theta += 2.0f*Pi; // in [0, 2*pi).
//	}
//
//	// Quadrant II or III
//	else      
//		theta = atanf(y/x) + Pi; // in [0, 2*pi).
//
//	return theta;
//}
//
//XMVECTOR MathHelper::RandUnitVec3()
//{
//	XMVECTOR One  = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
//	XMVECTOR Zero = XMVectorZero();
//
//	// Keep trying until we get a point on/in the hemisphere.
//	while(true)
//	{
//		// Generate random point in the cube [-1,1]^3.
//		XMVECTOR v = XMVectorSet(MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), 0.0f);
//
//		// Ignore points outside the unit sphere in order to get an even distribution 
//		// over the unit sphere.  Otherwise points will clump more on the sphere near 
//		// the corners of the cube.
//
//		if( XMVector3Greater( XMVector3LengthSq(v), One) )
//			continue;
//
//		return XMVector3Normalize(v);
//	}
//}
//
//XMVECTOR MathHelper::RandHemisphereUnitVec3(XMVECTOR n)
//{
//	XMVECTOR One  = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
//	XMVECTOR Zero = XMVectorZero();
//
//	// Keep trying until we get a point on/in the hemisphere.
//	while(true)
//	{
//		// Generate random point in the cube [-1,1]^3.
//		XMVECTOR v = XMVectorSet(MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), 0.0f);
//
//		// Ignore points outside the unit sphere in order to get an even distribution 
//		// over the unit sphere.  Otherwise points will clump more on the sphere near 
//		// the corners of the cube.
//		
//		if( XMVector3Greater( XMVector3LengthSq(v), One) )
//			continue;
//
//		// Ignore points in the bottom hemisphere.
//		if( XMVector3Less( XMVector3Dot(n, v), Zero ) )
//			continue;
//
//		return XMVector3Normalize(v);
//	}
//}
//
//void MathHelper::OrthoNormalInverse( XMFLOAT4X4& MatrixO, const XMFLOAT4X4& MatrixA )
//{
//	MatrixO = MatrixA;
//	MatrixO._12 = MatrixA._21;
//	MatrixO._13 = MatrixA._31;
//	MatrixO._21 = MatrixA._12;
//	MatrixO._23 = MatrixA._32;
//	MatrixO._31 = MatrixA._13;
//	MatrixO._32 = MatrixA._23;
//
//	
//	axis = XMFLOAT3(MatrixA._41,MatrixA._42,MatrixA._43);
//
//	XMVECTOR axis_pos = XMLoadFloat3(&axis);
//	XMVECTOR axis_x = XMLoadFloat3(&XMFLOAT3(MatrixA._11,MatrixA._12,MatrixA._13));
//	XMVECTOR axis_y = XMLoadFloat3(&XMFLOAT3(MatrixA._21,MatrixA._22,MatrixA._23));
//	XMVECTOR axis_z = XMLoadFloat3(&XMFLOAT3(MatrixA._31,MatrixA._32,MatrixA._33));
//
//	XMFLOAT3 result;
//	XMStoreFloat3(&result, XMVector3Dot(axis_pos, axis_x));
//	MatrixO._41 = -result.x;
//
//	XMStoreFloat3(&result, XMVector3Dot(axis_pos, axis_y));
//	MatrixO._42 = -result.x;
//
//	XMStoreFloat3(&result, XMVector3Dot(axis_pos, axis_z));
//	MatrixO._43 = -result.x;
//}
//
//XMVECTOR& MathHelper::VectorByMatrixMult(XMVECTOR& vector, XMFLOAT4X4& matrix, XMVECTOR& returnVec)
//{
//	XMFLOAT3 result;	
//	XMStoreFloat3(&result, XMVector3Dot(vector, XMLoadFloat3(&XMFLOAT3(matrix._11,matrix._21,matrix._31))));
//	float outX = result.x + matrix._41;
//
//	XMStoreFloat3(&result, XMVector3Dot(vector, XMLoadFloat3(&XMFLOAT3(matrix._12,matrix._22,matrix._32))));
//	float outY = result.x + matrix._42;
//
//	XMStoreFloat3(&result, XMVector3Dot(vector, XMLoadFloat3(&XMFLOAT3(matrix._13,matrix._23,matrix._33))));
//	float outZ = result.x + matrix._43;
//
//	returnVec = XMLoadFloat3(&XMFLOAT3(outX,outY,outZ));
//
//	return returnVec;
//}
//
//void MathHelper::InterpolateMatrices(XMFLOAT4X4& _prevFrame, XMFLOAT4X4& _nextFrame, float _lambda, XMFLOAT4X4& _result)
//{
//	//_result = _prevFrame;
//
//	XMMATRIX matrixA = XMLoadFloat4x4(&_prevFrame);
//	XMMATRIX matrixB = XMLoadFloat4x4(&_nextFrame);
//
//	XMVECTOR determinant = XMMatrixDeterminant(matrixA);
//	XMMATRIX invMat = XMMatrixInverse(&determinant, matrixA);
//
//	XMMATRIX result = XMLoadFloat4x4(&_nextFrame);
//
//	result = XMMatrixMultiply(result,invMat);
//
//	invMat = result;
//	
//	float trace = 1.0f + invMat._11 + invMat._22 + invMat._33;
//
//	XMFLOAT4 quatResult;
//	
//	if(trace > 0.00000001f)
//	{
//		float S = (float)sqrt(trace)*2;
//		quatResult.x = (invMat._32 - invMat._23) / S;
//		quatResult.y = (invMat._13 - invMat._31) / S;
//		quatResult.z = (invMat._21 - invMat._12) / S;
//		quatResult.w = 0.25f * S;
//	}
//	else
//	{
//		if( invMat._11 > invMat._22 && invMat._11 > invMat._33 )
//		{
//			float S = (float)sqrt( 1.0 + invMat._11 - invMat._22 - invMat._33 ) * 2;
//			quatResult.x = 0.25f * S;
//			quatResult.y = (invMat._21 - invMat._12) / S;
//			quatResult.z = (invMat._13 - invMat._31) / S;
//			quatResult.w = (invMat._21 - invMat._12) / S;
//		}
//		else if( invMat._22 > invMat._33 )
//		{
//			float S = (float)sqrt( 1.0 + invMat._22 - invMat._11 - invMat._33 ) * 2;
//			quatResult.x = (invMat._21 - invMat._12) / S;
//			quatResult.y = 0.25f * S;
//			quatResult.z = (invMat._21 - invMat._12) / S;
//			quatResult.w = (invMat._13 - invMat._31) / S;
//		}
//		else
//		{
//			float S = (float)sqrt( 1.0 + invMat._33 - invMat._11 - invMat._22 ) * 2;
//			quatResult.x = (invMat._13 - invMat._31) / S;
//			quatResult.y = (invMat._21 - invMat._12) / S;
//			quatResult.z = 0.25f * S;
//			quatResult.w = (invMat._21 - invMat._12) / S;
//		}
//	}
//
//	XMVECTOR quaternion = XMLoadFloat4(&quatResult);
//
//	XMVECTOR quaternionNorm = XMQuaternionNormalize(quaternion);
//
//	XMFLOAT4 quatNorm;
//	XMStoreFloat4(&quatNorm, quaternionNorm);
//
//	float cos_a = quatNorm.w;
//	float angle = acos( cos_a ) * 2;
//	float sin_a = sqrt( 1.0f - cos_a * cos_a );
//
//	if( angle == 0.0 )
//	{
//		result = matrixA;
//
//		result.m[3][0] = matrixA.m[3][0] + ((matrixB.m[3][0]-matrixA.m[3][0])*_lambda);
//		result.m[3][1] = matrixA.m[3][1] + ((matrixB.m[3][1]-matrixA.m[3][1])*_lambda);
//		result.m[3][2] = matrixA.m[3][2] + ((matrixB.m[3][2]-matrixA.m[3][2])*_lambda);
//
//		XMStoreFloat4x4(&_result, result);
//		return;
//	}
//
//	XMFLOAT3 axis;
//
//	if( fabs( sin_a ) < 0.0005 )
//		sin_a = 1;
//
//	axis.x = quatNorm.x / sin_a;
//	axis.y = quatNorm.y / sin_a;
//	axis.z = quatNorm.z / sin_a;
//
//	angle *= _lambda;
//
//	XMVECTOR axisVec = XMLoadFloat3(&axis);
//	axisVec = XMVector3Normalize(axisVec);
//
//	XMStoreFloat3(&axis, axisVec);
//
//	sin_a = sin( angle / 2 );
//	cos_a = cos( angle / 2 );
//	quatNorm.x = axis.x * sin_a;
//	quatNorm.y = axis.y * sin_a;
//	quatNorm.z = axis.z * sin_a;
//	quatNorm.w = cos_a;
//
//	quaternionNorm = XMLoadFloat4(&quatNorm);
//
//	XMStoreFloat4(&quatNorm, XMQuaternionNormalize(quaternionNorm));
//
//	float xx      = quatNorm.x * quatNorm.x;
//	float xy      = quatNorm.x * quatNorm.y;
//	float xz      = quatNorm.x * quatNorm.z;
//	float xw      = quatNorm.x * quatNorm.w;
//	float yy      = quatNorm.y * quatNorm.y;
//	float yz      = quatNorm.y * quatNorm.z;
//	float yw      = quatNorm.y * quatNorm.w;
//	float zz      = quatNorm.z * quatNorm.z;
//	float zw      = quatNorm.z * quatNorm.w;
//
//	_result._11  = 1 - 2 * ( yy + zz );
//	_result._12  =     2 * ( xy - zw );
//	_result._13  =     2 * ( xz + yw );
//	_result._21  =     2 * ( xy + zw );
//	_result._22  = 1 - 2 * ( xx + zz );
//	_result._23  =     2 * ( yz - xw );
//	_result._31  =     2 * ( xz - yw );
//	_result._32  =     2 * ( yz + xw );
//	_result._33  = 1 - 2 * ( xx + yy );
//	_result._14  = result._24 = result._34 = result._41 = result._42 = result._43 = 0;
//	_result._44 = 1;
//
//	result = XMLoadFloat4x4(&_result);
//	result = XMMatrixMultiply(result, matrixA);
//
//	XMStoreFloat4x4(&_result, result);
//
//	//interpolate position
//	_result.m[3][0] = matrixA.m[3][0] + ((matrixB.m[3][0]-matrixA.m[3][0])*_lambda);
//	_result.m[3][1] = matrixA.m[3][1] + ((matrixB.m[3][1]-matrixA.m[3][1])*_lambda);
//	_result.m[3][2] = matrixA.m[3][2] + ((matrixB.m[3][2]-matrixA.m[3][2])*_lambda);
//}
//
//float MathHelper::CalcGaussianBlurWeight(float x)
//{
//	const float StandardDeviation = 1;
//	const float StandardDeviationSquared = 
//		StandardDeviation * StandardDeviation;
//
//	return (1.0f / sqrt(2 * MathHelper::Pi * StandardDeviationSquared)) * 
//		pow(float(M_E), float(-((x * x) / (2 * StandardDeviationSquared))));
//}