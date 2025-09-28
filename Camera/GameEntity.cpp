#include "GameEntity.h"
#include "Mesh.h"
#include "Transform.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include "Vertex.h"
#include <DirectXMath.h>

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
void GameEntity::Draw() {
	mesh->Draw();
}