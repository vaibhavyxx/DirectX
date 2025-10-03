#include "Camera.h"
#include "Transform.h"
#include <DirectXMath.h>
#include "Input.h"
#include <algorithm>
#include <iostream>

Camera::Camera(float aspectRatio, DirectX::XMFLOAT3 initialPosition, float fieldOfView, float nearClip, float farClip, float movementSpeed, float mouseLookSpeed, bool orthographic)
{
    this->aspectRatio = aspectRatio;
    this->fov = fieldOfView;
    this->nearClip = nearClip;
    this->farClip = farClip;
    this->orthographic = orthographic;
    this->moveSpeed = movementSpeed;
    this->mouseLookSpeed = mouseLookSpeed;
    transform = std::make_shared<Transform>();
    transform->SetPosition(initialPosition);

    UpdateViewMatrix();
    UpdateProjectionMatrix(aspectRatio);
}

DirectX::XMFLOAT4X4 Camera::GetView()
{
    return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjection()
{
    return projectionMatrix;
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
    this->aspectRatio = aspectRatio;    //For window resizing
    DirectX::XMMATRIX projection;
    if (!orthographic) {
        projection = DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClip, farClip);
    }
    else {
        projection = DirectX::XMMatrixOrthographicLH(100, 100, nearClip, farClip);
    }
    DirectX::XMStoreFloat4x4(&projectionMatrix, projection);
}

void Camera::UpdateViewMatrix()
{
    DirectX::XMFLOAT3 forward = transform->GetForward();
    DirectX::XMFLOAT3 pos = transform->GetPosition();

    DirectX::XMMATRIX view =
        DirectX::XMMatrixLookToLH(
            DirectX::XMLoadFloat3(&pos),
            DirectX::XMLoadFloat3(&forward),
            DirectX::XMVectorSet(0, 1, 0, 0));

    DirectX::XMStoreFloat4x4(&viewMatrix, view);
}

void Camera::Update(float dt)
{
    float distance = moveSpeed * dt;

    //Relative movement
    if (Input::KeyDown('W'))
        transform->MoveRelative(0, 0, distance);
    if (Input::KeyDown('S')) 
        transform->MoveRelative(0, 0, -distance);
    if (Input::KeyDown('A'))
        transform->MoveRelative(-distance, 0, 0);
    if (Input::KeyDown('D'))
        transform->MoveRelative(distance, 0, 0);

    auto pos = transform->GetPosition();
    std::cout << "camera: (" << pos.x << ", " << pos.y << ", " << pos.z << ")\n";

    //Absolute movement
    if (Input::KeyDown(VK_SPACE))
        transform->MoveAbsolute(0, -distance, 0);
    if (Input::KeyDown('X'))
        transform->MoveAbsolute(0, distance, 0);

    if (Input::MouseLeftDown()) {
        int cursorMovementX = Input::GetMouseXDelta() * mouseLookSpeed;
        int cursorMovementY = Input::GetMouseYDelta() * mouseLookSpeed;
        transform->Rotate(cursorMovementY, cursorMovementX, 0.0f);

        DirectX::XMFLOAT3 pitchYawRoll = transform->GetPitchYawRoll();
        if (pitchYawRoll.x > DirectX::XM_PIDIV2)
            pitchYawRoll.x = DirectX::XM_PIDIV2;

        if( pitchYawRoll.x < (DirectX::XM_PIDIV2 * -1.0f)) 
            pitchYawRoll.x = -DirectX::XM_PIDIV2;
       
        transform->SetRotation(pitchYawRoll);
    }
    UpdateViewMatrix();
}
