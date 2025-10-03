#include "GameEntity.h"
#include "Mesh.h"
#include "Transform.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include "Vertex.h"
#include <DirectXMath.h>
#include "Camera.h"
#include "Graphics.h"
#include <iostream>

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
void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, std::shared_ptr<Camera> camera) {
	
	ConstantBufferData cbData = {};
	cbData.worldMatrix = transform->GetWorldMatrix();
	cbData.viewMatrix = camera->GetView();
	cbData.projectionMatrix = camera->GetProjection();

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	Graphics::Context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &cbData, sizeof(cbData));
	Graphics::Context->Unmap(constantBuffer.Get(), 0);
	
	mesh->Draw();
}