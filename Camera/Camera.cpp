#include "Camera.h"
#include "Transform.h"
#include <DirectXMath.h>

Camera::Camera(float aspectRatio, DirectX::XMFLOAT3 initialPosition, float fieldOfView, float nearClip, float farClip, float movementSpeed, float mouseLookSpeed)
{
    this->aspectRatio = aspectRatio;
    this->fov = fieldOfView;
    this->nearClip = nearClip;
    this->farClip = farClip;
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
    UpdateViewMatrix();
}
