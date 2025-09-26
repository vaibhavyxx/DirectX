#include "Transform.h"
#include <DirectXMath.h>

Transform::Transform(): 
	f3Position(0,0,0),
	f3Rotation(0,0,0),
	f3Scale(1,1,1),
	updateMatrices(false)
{
	//Sets world and inverse world matrix to identity matrices
	DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&f4WorldInverseTranspose, DirectX::XMMatrixIdentity());
}

Transform::~Transform()
{
	//Nothing so far
}

void Transform::SetPosition(float x, float y, float z)
{
	f3Position.x = x;
	f3Position.y = y;
	f3Position.z = z;
	updateMatrices = true;
}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	this->f3Position = position;
	updateMatrices = true;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	f3Rotation.x = pitch;
	f3Rotation.y = yaw;
	f3Rotation.z = roll;
	updateMatrices = true;
}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{
	this->f3Rotation = rotation;
	updateMatrices = true;
}

void Transform::SetScale(float x, float y, float z)
{
	f3Scale.x = x;
	f3Scale.y = y;
	f3Scale.z = z;
	updateMatrices = true;
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	this->f3Scale = scale;
	updateMatrices = true;
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return f3Position;
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
	return f3Rotation;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return f3Scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (updateMatrices) {
		DirectX::XMMATRIX mTransform = XMMatrixTranslation();
		DirectX::XMMATRIX mScale = XMMatrixScaling();
		DirectX::XMMATRIX mRotate = XMMatrixRotationRollPitchYaw();
		DirectX::XMMATRIX mWorld = mScale * mRotate * mTransform;

		DirectX::XMStoreFloat4x4(&world, mWorld);
	}
	return world;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	if (updateMatrices) {
		DirectX::XMMATRIX mWorld = DirectX::XMLoadFloat4x4(&world);
		DirectX::XMStoreFloat4x4(&f4WorldInverseTranspose,
			DirectX::XMMatrixInverse(0, DirectX::XMMatrixTranspose(mWorld)));
	}
	return f4WorldInverseTranspose;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	f3Position.x += x;
	f3Position.y += y;
	f3Position.z += z;
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	DirectX::XMVECTOR tPos = DirectX::XMLoadFloat3(&f3Position);
	DirectX::XMVECTOR tOffset = DirectX::XMLoadFloat3(&offset);
	tPos = DirectX::XMVectorAdd(tPos, tOffset);
	DirectX::XMStoreFloat3(&f3Position, tPos);
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	f3Rotation.x += pitch;
	f3Rotation.y += yaw;
	f3Rotation.z += roll;
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation)
{
	DirectX::XMVECTOR tRot = DirectX::XMLoadFloat3(&f3Rotation);
	DirectX::XMVECTOR tOffset = DirectX::XMLoadFloat3(&rotation);
	tRot = DirectX::XMVectorAdd(tRot, tOffset);
	DirectX::XMStoreFloat3(&f3Rotation, tRot);
}

void Transform::Scale(float x, float y, float z)
{
	f3Scale.x *= x;
	f3Scale.y *= y;
	f3Scale.z *= z;
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
	DirectX::XMVECTOR tScale = DirectX::XMLoadFloat3(&f3Scale);
	DirectX::XMVECTOR tOffset = DirectX::XMLoadFloat3(&scale);
	tScale = DirectX::XMVectorMultiply(tScale, tOffset);
	DirectX::XMStoreFloat3(&f3Scale, tScale);
}

DirectX::XMMATRIX Transform::XMMatrixTranslation()
{
	DirectX::XMVECTOR vTranslate = DirectX::XMLoadFloat3(&f3Position);
	DirectX::XMMATRIX mTranslate = DirectX::XMMatrixTranslationFromVector(vTranslate);
	return mTranslate;
}

DirectX::XMMATRIX Transform::XMMatrixScaling()
{
	DirectX::XMVECTOR vScale = DirectX::XMLoadFloat3(&f3Scale);
	DirectX::XMMATRIX mScale = DirectX::XMMatrixScalingFromVector(vScale);
	return mScale;
}

DirectX::XMMATRIX Transform::XMMatrixRotationRollPitchYaw()
{
	DirectX::XMVECTOR vRotate = DirectX::XMLoadFloat3(&f3Rotation);
	DirectX::XMMATRIX mRotate = DirectX::XMMatrixTranslationFromVector(vRotate);
	return mRotate;
}
