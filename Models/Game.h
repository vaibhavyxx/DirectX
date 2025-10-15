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
	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constBuffer;

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	std::vector<GameEntity> entities;
	std::vector<std::shared_ptr<Camera>> cameras;
	std::vector<std::shared_ptr<Mesh>> meshes;
	ConstantBufferData cbData = {};

	float color[4] = { 0.4f, 0.6f, 0.75f, 1.0f };
	int currentCamera = 0;
};

