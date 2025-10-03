#include "Camera.h"
#include "Input.h"
#include <iostream>

Camera::Camera(DirectX::XMFLOAT3 position, float aspectRatio, 
    float fov, float nearClip, float farClip, float moveSpeed, 
    float mouseLookSpeed, float size, bool isProjection) 
{
    this->pos = position;
    this->aspectRatio = aspectRatio;
    this->fieldOfView = fov;
    this->nearClip = nearClip;
    this->farClip = farClip;
    this->movementSpeed = moveSpeed;
    this->mouseLookSpeed = mouseLookSpeed;
    this->orthographicSize = size;
    this->isProjection = isProjection;

    transform = std::make_shared<Transform>();
    transform->SetPosition(pos);

    UpdateViewMatrix();
    UpdateProjectionMatrix(aspectRatio);
}

DirectX::XMFLOAT4X4 Camera::GetView()
{
    return mView;
}

DirectX::XMFLOAT4X4 Camera::GetProjection()
{
    return mProj;
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
    XMMATRIX proj;
    if (isProjection) {
        proj = XMMatrixPerspectiveLH(
            fieldOfView, 
            aspectRatio,
            nearClip,
            farClip
        );
    }
    else {
        proj = XMMatrixOrthographicLH(
            orthographicSize,
            orthographicSize / aspectRatio,
            nearClip,
            farClip
        );
    }
    XMStoreFloat4x4(&mProj, proj);
}

void Camera::UpdateViewMatrix()
{
    XMFLOAT3 fwd = transform->GetForward();
    XMFLOAT3 position = transform->GetPosition();

    XMMATRIX view = XMMatrixLookToLH(
        XMLoadFloat3(&position),
        XMLoadFloat3(&fwd),
        XMVectorSet(0, 1, 0, 0)
    );
    XMStoreFloat4x4(&mView, view);
}

void Camera::Update(float dt)
{
    float dis = dt * movementSpeed;
    if (Input::KeyDown('W')) { transform->MoveRelative(0, 0, dis); }
    if (Input::KeyDown('S')) { transform->MoveRelative(0, 0, -dis); }
    if (Input::KeyDown('A')) { transform->MoveRelative(-dis, 0, 0); }
    if (Input::KeyDown('D')) { transform->MoveRelative(dis, 0, 0); }
    if (Input::KeyDown('X')) { transform->MoveAbsolute(0, -dis, 0); }
    if (Input::KeyDown(' ')) { transform->MoveAbsolute(0, dis, 0); }

    auto pos = transform->GetPosition();
    std::cout << "camera: (" << pos.x << ", " << pos.y << ", " << pos.z << ")\n";
    
    //Absolute movement
    if (Input::KeyDown(VK_SPACE))
        transform->MoveAbsolute(0, -dis, 0);
    if (Input::KeyDown('X'))
        transform->MoveAbsolute(0, dis, 0);

    if (Input::MouseLeftDown()) {
        int cursorMovementX = Input::GetMouseXDelta() * mouseLookSpeed;
        int cursorMovementY = Input::GetMouseYDelta() * mouseLookSpeed;
        transform->Rotate(cursorMovementY, cursorMovementX, 0.0f);

        DirectX::XMFLOAT3 pitchYawRoll = transform->GetPitchYawRoll();
        if (pitchYawRoll.x > DirectX::XM_PIDIV2)
            pitchYawRoll.x = DirectX::XM_PIDIV2;

        if (pitchYawRoll.x < (DirectX::XM_PIDIV2 * -1.0f))
            pitchYawRoll.x = -DirectX::XM_PIDIV2;

        transform->SetRotation(pitchYawRoll);
    }
    UpdateViewMatrix();
}
