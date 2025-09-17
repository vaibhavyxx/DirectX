#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Mesh.h"
#include <memory>
#include "Vertex.h"
#include <DirectXMath.h>
#include <vector>
class Game
{
public:
	// Basic OOP setup
	Game();
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

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

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	//Shared ptrs for mesh class
	std::shared_ptr<Mesh> triangle;
	std::shared_ptr<Mesh> quad;
	std::shared_ptr<Mesh> pentagon;
	std::shared_ptr<Mesh> hexagon;
	std::shared_ptr<Mesh> dodecagon;

};

