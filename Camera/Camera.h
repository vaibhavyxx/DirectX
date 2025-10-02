#pragma once
#include "Transform.h"
#include <DirectXMath.h>
#include <memory>

class Camera
{
public:
	Camera(float aspectRatio, 
		DirectX::XMFLOAT3 initialPosition,
		float fieldOfView,
		float nearClip,
		float farClip,
		float movementSpeed,
		float mouseLookSpeed);

	//Getters
	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();

	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float dt);

private:
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	DirectX::XMFLOAT3 position;
	std::shared_ptr<Transform> transform;

	//Camera values
	float fov;
	float nearClip;
	float farClip;
	float aspectRatio;

	//Camera type
	bool orthographic;

	//Input
	float moveSpeed;
	float mouseLookSpeed;
};

