#include "GameEntity.h"
#include "Mesh.h"
#include "Transform.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include "Vertex.h"
#include <DirectXMath.h>
#include <iostream>
#include "Camera.h"
#include "Graphics.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh): mesh(mesh)
{
	transform = std::make_shared<Transform>();
}

std::shared_ptr<Mesh> GameEntity::GetMesh() {
	return mesh;
}
std::shared_ptr<Transform> GameEntity::GetTransform() {
	return transform;
}

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer, std::shared_ptr<Camera> cam) {
	
	ConstantBufferData vsData = {};
	vsData.worldMatrix = transform->GetWorldMatrix();
	vsData.viewMatrix = cam->GetView();
	//vsData.projectionMatrix = cam->GetProjection();
	DirectX::XMStoreFloat4x4(&vsData.projectionMatrix, DirectX::XMMatrixIdentity());

	// Copy this data to the constant buffer we intend to use
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	Graphics::Context->Map(vsConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

	// Straight memcpy() into the resource
	memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));

	// Unmap so the GPU can once again use the buffer
	Graphics::Context->Unmap(vsConstantBuffer.Get(), 0);
	Graphics::Context->VSSetConstantBuffers(0, 1, vsConstantBuffer.GetAddressOf());
	mesh->Draw();
}