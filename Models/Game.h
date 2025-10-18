#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Mesh.h"
#include <memory>
#include "Vertex.h"
#include <DirectXMath.h>
#include <vector>
#include "Transform.h"
#include "GameEntity.h"
#include "Camera.h"
#include "Material.h"
#include "Shader.h"

class Game
{
public:
	// Basic OOP setup
	Game();
	~Game();
	//Game(const Game&) = delete; // Remove copy constructor
	//Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();
	void Initialize();
	
private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	void CreateGeometry();
	void FrameReset(float deltaTime);
	void BuildUI();
	void AppDetails();
	void MeshDetails(std::shared_ptr<Mesh> mesh, const char* name);
	void EntityValues(std::shared_ptr<Transform> entity, unsigned int i);
	void ConstantBufferUI();

	template<typename T, size_t N>
	int ArrayCount(const T (&array)[N]);
	std::vector<DirectX::XMFLOAT3> GenerateVertices(float centerX, float centerY, int sides, float radius);
	std::vector<unsigned int> GenerateIndices(int sides);
	
	Microsoft::WRL::ComPtr<ID3D11Buffer> constBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pixelBuffer;

	std::shared_ptr<Shader> pixelShader;
	std::shared_ptr<Shader> uvShader;
	std::shared_ptr<Shader> normalShader;

	std::vector<std::shared_ptr<GameEntity>> pixelEntities;
	std::vector<std::shared_ptr<GameEntity>> UVEntities;
	std::vector<std::shared_ptr<GameEntity>> normalEntities;

	std::vector<std::shared_ptr<Camera>> cameras;
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Material>> materials;
	ConstantBufferData cbData = {};

	float color[4] = { 0.4f, 0.6f, 0.75f, 1.0f };
	int currentCamera = 0;
};

