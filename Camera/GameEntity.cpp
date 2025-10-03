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

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer, std::shared_ptr<Camera> camera) {
	/*ConstantBufferData data = {};
	data.worldMatrix = transform->GetWorldMatrix();
	data.viewMatrix = camera->GetView();
	data.projectionMatrix = camera->GetProjection();

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	Graphics::Context->Map(vsConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &data, sizeof(data));
	Graphics::Context->Unmap(vsConstantBuffer.Get(), 0);
	*/
	mesh->Draw();
}