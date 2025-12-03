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
#include "Sky.h"
#include <string>

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
	void LoadLights(float offset);
	void LoadCameras();
	void CreateTextures();
	void CreateMaterials();
	void CreateGeometry();
	void FrameReset(float deltaTime);
	void BuildUI();
	void AppDetails();
	void MeshDetails(std::shared_ptr<Mesh> mesh, const char* name);
	void EntityValues(std::shared_ptr<GameEntity> entity, unsigned int i);
	void LoadTextures(std::string filepath, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv);
	void MaterialsUI();

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerStateOverlay;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	std::shared_ptr<Shader> shader;
	std::shared_ptr<Shader> skyShader;
	std::shared_ptr<Sky> sky;
	std::vector<std::shared_ptr<GameEntity>> gameEntities;
	
	std::vector<std::shared_ptr<Camera>> cameras;
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Material>> materials;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> srvVector;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> normalsSRV;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> skySRV;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> floorMaterials;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> metalMaterials;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> cobblestoneMaterials;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> textures[6] = {};

	float color[4] = { 0.4f, 0.6f, 0.75f, 1.0f };
	int currentCamera = 0;
	int currentLight = 0;
	DirectX::XMFLOAT3 ambientColor;
	Light lights[5];
	
	std::shared_ptr<GameEntity> floorGameObject;
	std::shared_ptr<Material> floorMaterial;

	int shadowMapResolution;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler;
	
	DirectX::XMFLOAT4X4 lightViewMatrix;
	DirectX::XMFLOAT4X4 lightProjectionMatrix;
};

