#include "Camera.h"
#include "Transform.h"
#include <DirectXMath.h>
#include "Input.h"
#include <algorithm>
#include <iostream>

Camera::Camera(float aspectRatio, DirectX::XMFLOAT3 initialPosition, 
    float fieldOfView, float nearClip, float farClip, float movementSpeed, float mouseLookSpeed,
    float windowSize,bool orthographic)
{
    this->aspectRatio = aspectRatio;
    this->fov = fieldOfView;
    this->nearClip = nearClip;
    this->farClip = farClip;
    this->orthographic = orthographic;
    this->moveSpeed = 5.0f;
    this->mouseLookSpeed = mouseLookSpeed;
    this->windowSize = windowSize;
    transform = std::make_shared<Transform>();
    transform->SetPosition(initialPosition);

    UpdateViewMatrix();
    UpdateProjectionMatrix(this->aspectRatio);
}

DirectX::XMFLOAT4X4 Camera::GetView()
{
    //UpdateViewMatrix();
    return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjection()
{
    //UpdateViewMatrix();
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
        projection = DirectX::XMMatrixOrthographicLH(this->orthographic, this->windowSize/aspectRatio, nearClip, farClip);
    }
    DirectX::XMStoreFloat4x4(&projectionMatrix, projection);
}

void Camera::UpdateViewMatrix()
{
    DirectX::XMFLOAT3 forward = transform->GetForward();
    DirectX::XMFLOAT3 pos = transform->GetPosition();
    DirectX::XMFLOAT3 right = transform->GetRight();
    DirectX::XMFLOAT3 up = transform->GetUp();

    DirectX::XMMATRIX view =
        DirectX::XMMatrixLookToLH(
            DirectX::XMLoadFloat3(&pos),
            DirectX::XMLoadFloat3(&forward),
            DirectX::XMVectorSet(0, 1, 0, 0));

    DirectX::XMStoreFloat4x4(&viewMatrix, view);
}

std::shared_ptr<Transform> Camera::GetTransform() { return transform; }
float Camera::GetFOV() { return this->fov; }
bool Camera::IsOrthographic() { return this->orthographic; }

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

    //Absolute movement
    if (Input::KeyDown(VK_SPACE))
        transform->MoveAbsolute(0, -distance, 0);
    if (Input::KeyDown('X'))
        transform->MoveAbsolute(0, distance, 0);

    if (Input::MouseLeftDown()) {

        float cursorMovementX = Input::GetMouseXDelta() * this->mouseLookSpeed;
        float cursorMovementY = Input::GetMouseYDelta() * this->mouseLookSpeed;
        transform->Rotate(cursorMovementY, cursorMovementX, 0.0f);
        //std::cout << "mouse: " << cursorMovementX << ", " << cursorMovementY << "\n";

        DirectX::XMFLOAT3 pitchYawRoll = transform->GetPitchYawRoll();
        if (pitchYawRoll.x > DirectX::XM_PIDIV2)
            pitchYawRoll.x = DirectX::XM_PIDIV2;

        if( pitchYawRoll.x < (-DirectX::XM_PIDIV2)) 
            pitchYawRoll.x = -DirectX::XM_PIDIV2;
        //std::cout << "mouse: " << cursorMovementX << ", " << cursorMovementY <<"\n";
        transform->SetRotation(pitchYawRoll);
        DirectX::XMFLOAT3 pyr = transform->GetPitchYawRoll();
        //  if(pyr.x > 0 || pyr.y >0 || pyr.z >0)
        //      std::cout << "rotation: " << pyr.x << ", " << pyr.y <<", "<< pyr.z << "\n";
    }
    UpdateViewMatrix();
}
