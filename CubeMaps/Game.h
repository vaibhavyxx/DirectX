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
#include "Lights.h"

class Game
{
public:
	// Basic OOP setup
	Game();
	~Game();

	// Primary functions
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();
	void Initialize();

private:
	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void CreateGeometry();
	void FrameReset(float deltaTime);
	void BuildUI();
	void AppDetails();
	void MeshDetails(std::shared_ptr<Mesh> mesh, const char* name);
	void EntityValues(std::shared_ptr<GameEntity> entity, unsigned int i);

	/*Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srvRock;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srvWater;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srvOverlay;*/
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerStateOverlay;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	std::shared_ptr<Shader> shaders;
	std::vector<std::shared_ptr<GameEntity>> gameEntities;
	
	std::vector<std::shared_ptr<Camera>> cameras;
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Material>> materials;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> srvVector;

	float color[4] = { 0.4f, 0.6f, 0.75f, 1.0f };
	int currentCamera = 0;
	int currentLight = 0;
	DirectX::XMFLOAT3 ambientColor;
	Light lights[5];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> crate;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> water;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rock;
};

