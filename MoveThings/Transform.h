#pragma once
#include <DirectXMath.h>
class Transform
{
public:
	Transform();
	Transform(const Transform&) = delete;
	Transform& operator = (const Transform&) = delete;
	~Transform();

	//Setters
	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3 position);

	void SetRosition(float pitch, float yaw, float roll);
	void SetRosition(DirectX::XMFLOAT3 rotation); //Quaternion for future?

	void SetScale(float x, float y, float z);
	void SetScale(DirectX::XMFLOAT3 scale);

	//Getters
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetPitchYawRoll();	//Quaternion for future
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();

	//Transformers
	void MoveAbsolute(float x, float y, float z);
	void MoveAbsolute(DirectX::XMFLOAT3 offset);

	void Rotate(float pitch, float yaw, float roll);
	void Rotate(DirectX::XMFLOAT3 rotation); //Quaternion for future?

	void Scale(float x, float y, float z);
	void Scale(DirectX::XMFLOAT3 scale);

	DirectX::XMMATRIX XMMatrixTranslation();
	DirectX::XMMATRIX XMMatrixScaling();
	DirectX::XMMATRIX XMMatrixRotationRollPitchYaw();

private:
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 worldInverseTranspose;
	DirectX::XMFLOAT3 transformPosition;
	DirectX::XMFLOAT3 transformRotation;
	DirectX::XMFLOAT3 transformScale;


};

