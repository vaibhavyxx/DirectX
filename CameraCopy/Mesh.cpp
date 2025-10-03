#include "Mesh.h"
#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"

#include <DirectXMath.h>

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include "BufferStructs.h"

// This code assumes files are in "ImGui" subfolder!
// Adjust as necessary for your own folder structure and project setup
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

// For the DirectX Math library
using namespace DirectX;


Mesh::Mesh(Vertex vertices[],int vertexCount,unsigned int indices[], int indexCount) {
	this->verticesCount = vertexCount;
	this->indicesCount = indexCount;

	//Vertex buffer
	/* {
		D3D11_BUFFER_DESC vbd = {};
		vbd.Usage = D3D11_USAGE_IMMUTABLE;						// Will NEVER change
		vbd.ByteWidth = sizeof(Vertex) * verticesCount;			// 3 = number of vertices in the buffer
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;				// Tells Direct3D this is a vertex buffer
		vbd.CPUAccessFlags = 0;									// Note: We cannot access the data from C++ (this is good)
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initialVertexData = {};
		initialVertexData.pSysMem = vertices; // pSysMem = Pointer to System Memory
		Graphics::Device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());
	}
	//Index buffer
	{
		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(unsigned int) * indicesCount;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;									
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initialBufferData = {};
		initialBufferData.pSysMem = indices; // pSysMem = Pointer to System Memory
		Graphics::Device->CreateBuffer(&ibd, &initialBufferData, indexBuffer.GetAddressOf());
	}*/
}

Mesh::~Mesh() {
	//Nothing needs to be destroyed
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer() {
	return vertexBuffer;
}
Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer() {
	return indexBuffer;
}
int Mesh::GetIndexCount() {
	return Mesh::indicesCount;
}
int Mesh::GetVertexCount() {
	return Mesh::verticesCount;
}

void Mesh::Draw() {
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
		Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		
		Graphics::Context->DrawIndexed(
			this->indicesCount,		// The number of indices to use (we could draw a subset if we wanted)
			0,					// Offset to the first index we want to use
			0);					// Offset to add to each index when looking up vertices
	}
}

void Mesh::ConstBuffUI() {
	//ImGui::SliderFloat3("Offset", &cbData.offset.x, -1.0f, 1.0f);
	//ImGui::ColorEdit4("Color Tint", &cbData.colorTint.x);
}