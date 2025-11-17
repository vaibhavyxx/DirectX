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
	Mesh(const char* fileName);
	Mesh(const Mesh&) = delete;					//Removes copy constructor
	Mesh& operator = (const Mesh&) = delete;	//Removes copy assignment constructor
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

public:
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	int GetVertexCount();
	void Draw();
	int indicesCount;
	int verticesCount;
	~Mesh();
	void CreateBuffers(Vertex* vertArray, size_t numVerts, unsigned int* indexArray, size_t numIndices);
};

