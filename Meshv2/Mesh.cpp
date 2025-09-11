#include "Mesh.h"
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

// This code assumes files are in "ImGui" subfolder!
// Adjust as necessary for your own folder structure and project setup
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

// For the DirectX Math library
using namespace DirectX;

Mesh::Mesh(Vertex vertices[], int indices[]) {

}

Mesh::~Mesh() {

}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer() {
	return NULL;
}
Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer() {
	return NULL;
}
int Mesh::GetIndexCount() {
	return Mesh::indicesCount;
}
int Mesh::GetVertexCount() {
	return Mesh::verticesCount;
}
void Mesh::Draw(float deltaTime, float totalTime) {

}