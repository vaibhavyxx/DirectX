#include "Transform.h"
#include <DirectXMath.h>

Transform::Transform()
{
	transformPosition = DirectX::XMFLOAT3(0, 0, 0);
	transformRotation = DirectX::XMFLOAT3(0, 0, 0);
	transformScale	  = DirectX::XMFLOAT3(1, 1, 1);
}

Transform::~Transform()
{
	//Nothing so far
}

void Transform::SetPosition(float x, float y, float z)
{
	transformPosition.x = x;
	transformPosition.y = y;
	transformPosition.z = z;
}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	transformPosition = position;
}

void Transform::SetRosition(float pitch, float yaw, float roll)
{
	transformRotation.x = pitch;
	transformRotation.y = yaw;
	transformRotation.z = roll;
}

void Transform::SetRosition(DirectX::XMFLOAT3 rotation)
{
	transformRotation = rotation;
}

void Transform::SetScale(float x, float y, float z)
{
	transformScale.x = x;
	transformScale.y = y;
	transformScale.z = z;
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	transformScale = scale;
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return transformPosition;
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
	return transformRotation;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return transformScale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	return DirectX::XMFLOAT4X4();
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	return DirectX::XMFLOAT4X4();
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	transformPosition.x += x;
	transformPosition.y += y;
	transformPosition.z += z;
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	DirectX::XMVECTOR tPos = DirectX::XMLoadFloat3(&transformPosition);
	DirectX::XMVECTOR tOffset = DirectX::XMLoadFloat3(&offset);
	tPos = DirectX::XMVectorAdd(tPos, tOffset);
	DirectX::XMStoreFloat3(&transformPosition, tPos);
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	transformRotation.x += pitch;
	transformRotation.y += yaw;
	transformRotation.z += roll;
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation)
{
	DirectX::XMVECTOR tRot = DirectX::XMLoadFloat3(&transformRotation);
	DirectX::XMVECTOR tOffset = DirectX::XMLoadFloat3(&rotation);
	tRot = DirectX::XMVectorAdd(tRot, tOffset);
	DirectX::XMStoreFloat3(&transformRotation, tRot);
}

void Transform::Scale(float x, float y, float z)
{
	transformScale.x *= x;
	transformScale.y *= y;
	transformScale.z *= z;
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
	DirectX::XMVECTOR tScale = DirectX::XMLoadFloat3(&transformScale);
	DirectX::XMVECTOR tOffset = DirectX::XMLoadFloat3(&scale);
	tScale = DirectX::XMVectorMultiply(tScale, tOffset);
	DirectX::XMStoreFloat3(&transformScale, tScale);
}

DirectX::XMMATRIX Transform::XMMatrixTranslation()
{
	return DirectX::XMMATRIX();
}

DirectX::XMMATRIX Transform::XMMatrixScaling()
{
	return DirectX::XMMATRIX();
}

DirectX::XMMATRIX Transform::XMMatrixRotationRollPitchYaw()
{
	return DirectX::XMMATRIX();
}
