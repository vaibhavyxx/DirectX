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
#include "Shader.h"

class GameEntity
{
public:
	GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr <Material> material, std::shared_ptr<Shader> shader);
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
	std::shared_ptr<Material> GetMaterial();
	void Draw(std::shared_ptr<Camera> cam);	
	void Update(float deltaTime, float totalTime);
	
private:
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Material> material;
	std::shared_ptr<Shader> shader;

};

