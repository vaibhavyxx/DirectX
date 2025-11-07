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
#include "Shader.h"
#include "Lights.h"

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
std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return material;
}
void GameEntity::Update(float deltaTime, float time) {
	material->SetTime(time);
}

void GameEntity::Draw(std::shared_ptr<Camera> cam, Light* lights, DirectX::XMFLOAT3 color)
{
	material->GetShader()->Setup();
	material->BindTexturesAndSamplers();

	VertexStruct vsData = {};
	vsData.worldMatrix = transform->GetWorldMatrix();
	vsData.viewMatrix = cam->GetView();
	vsData.projectionMatrix = cam->GetProjection();
	vsData.worldInvTranspose = transform->GetWorldInverseTransposeMatrix();
	vsData.worldPos = transform->GetPosition();	//??
	Graphics::FillAndBindNextCB(&vsData, sizeof(VertexStruct), D3D11_VERTEX_SHADER, 0);
	
	material->SetAmbient(color);
	material->SetupPixelStruct(cam, lights);
	mesh->Draw();
}