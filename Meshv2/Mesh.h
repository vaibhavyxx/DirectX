#include "Vertex.h"
#include <d3d11.h>
#include <wrl/client.h>

#pragma once
class Mesh
{
	//Basics of OOP
	Mesh(Vertex vertices[], int indices[]);
	~Mesh();
	Mesh(const Mesh&) = delete;					//Removes copy constructor
	Mesh& operator = (const Mesh&) = delete;	//Removes copy assignment constructor

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

public:
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	int GetVertexCount();
	void Draw(float deltaTime, float totalTime);
	int indicesCount;
	int verticesCount;
};

