#pragma once
#include "Mesh.h"
#include "Transform.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include "Vertex.h"
#include <DirectXMath.h>
#include "Camera.h"

class GameEntity
{
public:
	GameEntity(std::shared_ptr<Mesh> mesh);
	//~GameEntity();
	//GameEntity(const GameEntity&) = delete; // Remove copy constructor
	//GameEntity& operator=(const GameEntity&) = delete; // Remove copy-assignment operator
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
	void Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer, std::shared_ptr<Camera> camera);	//do not set up cb here as it will cumbersome in the future

private:
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Transform> transform;
};

