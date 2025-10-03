#pragma once
#include "Transform.h"
#include <DirectXMath.h>
#include <memory>

using namespace DirectX;
class Camera
{
public:
	Camera(
		DirectX::XMFLOAT3 position,
		float aspectRatio,
		float fov,
		float nearClip,
		float farClip,
		float moveSpeed,
		float mouseLookSpeed,
		float size,
		bool isProjection
	);

	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();

	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float dt);

private:
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	XMFLOAT3 pos;
	std::shared_ptr<Transform> transform;

	//Values
	float fieldOfView;
	float aspectRatio;
	float nearClip;
	float farClip;
	float orthographicSize;
	bool isProjection;

	float movementSpeed;
	float mouseLookSpeed;
};

