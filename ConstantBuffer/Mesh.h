#include "Vertex.h"
#include <d3d11.h>
#include <wrl/client.h>
#include "BufferStructs.h"

#pragma once
class Mesh
{
public:
	//Basics of OOP
	Mesh(Vertex vertices[], int vertexCount, unsigned int indices[], int indexCount);
	Mesh(const Mesh&) = delete;					//Removes copy constructor
	Mesh& operator = (const Mesh&) = delete;	//Removes copy assignment constructor

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constBuffer;

public:
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	int GetVertexCount();
	void Draw();
	int indicesCount;
	int verticesCount;
	~Mesh();
};

