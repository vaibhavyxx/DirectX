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

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh,
	std::shared_ptr <Material> material) 
{
	this->mesh = mesh;
	this->material = material;
	transform = std::make_shared<Transform>();
}

std::shared_ptr<Mesh> GameEntity::GetMesh() {
	return mesh;
}
std::shared_ptr<Transform> GameEntity::GetTransform() {
	return transform;
}


void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> pixelBuffer,
	std::shared_ptr<Camera> cam) 
{
	material->MaterialSetup(vsConstantBuffer, pixelBuffer,transform, cam);
	mesh->Draw();
}