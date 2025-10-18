#pragma once
#include "Mesh.h"
#include "Transform.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include "Vertex.h"
#include <DirectXMath.h>
#include "Camera.h"
#include "Material.h"

class GameEntity
{
public:
	GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr <Material> material);
	//~GameEntity();
	//GameEntity(const GameEntity&) = delete; // Remove copy constructor
	//GameEntity& operator=(const GameEntity&) = delete; // Remove copy-assignment operator
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
	std::shared_ptr<Material> GetMaterial();

	//void SetMaterial(std::shared_ptr<Material> newMaterial);
	void Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> pixelBuffer, std::shared_ptr<Camera> cam);	//do not set up cb here as it will cumbersome in the future

private:
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Material> material;
};

