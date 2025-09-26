#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "Mesh.h"
#include "PathHelpers.h"
#include "Window.h"
#include <vector>
#include <DirectXMath.h>
#include <iostream>

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include "BufferStructs.h"
#include "Transform.h"
#include "GameEntity.h"

// This code assumes files are in "ImGui" subfolder!
// Adjust as necessary for your own folder structure and project setup
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// The constructor is called after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
Game::Game()
{
	Initialize();	
	LoadShaders();
	CreateGeometry();
	{
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Graphics::Context->IASetInputLayout(inputLayout.Get());
		Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
		Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);
	}
}

void Game::Initialize() {
	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();

	unsigned int size = sizeof(ConstantBufferData);
	size = (size + 15) / 16 * 16;

	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.ByteWidth = size;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;

	HRESULT hr = Graphics::Device->CreateBuffer(&cbDesc, 0, constBuffer.GetAddressOf());
	if (FAILED(hr)) {
		OutputDebugStringA("Failed to create a constant buffer!");
	}
}
// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU
		Graphics::Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
			pixelShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

		Graphics::Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			2,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}
}

//--------------------------------------------------------
//Helper function for count
//--------------------------------------------------------
template<typename T, size_t N>
int Game::ArrayCount(const T(&array)[N]) {
	return sizeof(array) / sizeof(array[0]);
}

//--------------------------------------------------------
//Generates Vertices for Polygons
//--------------------------------------------------------

std::vector<DirectX::XMFLOAT3> Game::GenerateVertices(float centerX, float centerY, int sides, float radius) {
	std::vector <DirectX::XMFLOAT3> tempVertex;

	for (int i = 0; i < sides; ++i) {
		float angle = (XM_PI * 2.0f) / sides * i;
		float x = centerX + cosf(angle) * radius;
		float y = centerY + sinf(angle) * radius;
		tempVertex.push_back(XMFLOAT3(x, y, 0.0f));
	}
	tempVertex.push_back(XMFLOAT3(centerX, centerY, 0.0f));
	return tempVertex;
}
//----------------------------------------------------------
//Generates indices
//---------------------------------------------------------
std::vector<unsigned int> Game::GenerateIndices(int sides) {
	int vertices = sides + 1;
	int totalIndices = vertices * 3;
	int centerIndex = sides;			//last index is the center index
	std::vector<unsigned int> indices;

	//Iterates through all the sides excluding the center
	for (int i = 0; i < sides; i++) {
		indices.push_back(i);
		indices.push_back(centerIndex);
		indices.push_back((i + 1) % sides);
	}
	return indices;
}

// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

#pragma region Triangle
	{
		Vertex vertices[] =
		{
			{ XMFLOAT3(+0.0f, +0.5f, +0.0f), red },
			{ XMFLOAT3(+0.5f, -0.5f, +0.0f), blue },
			{ XMFLOAT3(-0.5f, -0.5f, +0.0f), green },
		};
		unsigned int indices[] = { 0, 1, 2 };
		int vCount = ArrayCount(vertices);
		int iCount = ArrayCount(indices);
		std::shared_ptr<Mesh> mesh1 = (std::make_shared<Mesh>(vertices, vCount, indices, iCount));
		meshes.push_back(mesh1);
	}
#pragma endregion
#pragma region Square
	{
		//Square
		Vertex vertices[] = {
			{XMFLOAT3(+0.50f, +0.50f, +0.0f), blue},
			{XMFLOAT3(+0.75f, +0.50f, +0.0f), red},
			{XMFLOAT3(+0.50f, -0.50f, +0.0f), red},
			{XMFLOAT3(+0.75f, -0.50f, +0.0f), blue}
		};
		unsigned int indices[] = { 0, 1, 2, 2,1,3 };
		int vCount = ArrayCount(vertices);
		int iCount = ArrayCount(indices);
		std::shared_ptr<Mesh> mesh2 = std::make_shared<Mesh>(vertices, vCount, indices, iCount);
		meshes.push_back(mesh2);
	}
#pragma endregion
#pragma region Pentagon
	{
		const int sides = 5;
		const int totalVertex = sides + 1;		//including center
		Vertex vertices[totalVertex] = {};
		unsigned int indices[sides * 3];
		std::vector<DirectX::XMFLOAT3> pentagonVertices = GenerateVertices(-0.5f, 0.5f, sides, 0.25f);
		std::vector<unsigned int> pentagonIndices = GenerateIndices(sides);
		for (int i = 0; i < totalVertex; i++) {
			vertices[i] = { pentagonVertices[i], red };
		}
		for (int i = 0; i < (sides * 3); i++) {
			indices[i] = pentagonIndices[i];
		}
		std::shared_ptr<Mesh> mesh3 = (std::make_shared<Mesh>(vertices, totalVertex, indices, sides * 3));
		meshes.push_back(mesh3);
	}
#pragma endregion
#pragma region Hexagon
	{
		const int sides = 6;
		const int totalVertex = sides + 1;		//including center
		Vertex vertices[totalVertex] = {};
		unsigned int indices[sides * 3];
		std::vector<DirectX::XMFLOAT3> hexagonVerts = GenerateVertices(0.25f, 0.8f, sides, 0.15f);
		std::vector<unsigned int> hexIndices = GenerateIndices(sides);
		for (int i = 0; i < totalVertex; i++) {
			vertices[i] = { hexagonVerts[i], green };
		}
		for (int i = 0; i < (sides * 3); i++) {
			indices[i] = hexIndices[i];
		}
		std::shared_ptr<Mesh> mesh4 = (std::make_shared<Mesh>(vertices, totalVertex, indices, sides * 3));
		meshes.push_back(mesh4);
	}
#pragma endregion

#pragma region Octagon
	{
		const int sides = 8;
		const int totalVertex = sides + 1;		//including center
		Vertex vertices[totalVertex] = {};
		unsigned int indices[sides * 3];
		std::vector<DirectX::XMFLOAT3> hexagonVerts = GenerateVertices(-0.25f, 0.5f, sides, 0.15f);
		std::vector<unsigned int> hexIndices = GenerateIndices(sides);
		for (int i = 0; i < totalVertex; i++) {
			vertices[i] = { hexagonVerts[i], green };
		}
		for (int i = 0; i < (sides * 3); i++) {
			indices[i] = hexIndices[i];
		}
		std::shared_ptr<Mesh> mesh4 = (std::make_shared<Mesh>(vertices, totalVertex, indices, sides * 3));
		meshes.push_back(mesh4);
	}
#pragma endregion


	const int meshCount = meshes.size();
	//Setting it up 5 entities
	for (int i = 0; i < 7; i++) {
		if (i < meshCount) {
			entities.push_back(meshes[i]);
		}
		else {
			entities.push_back(entities[(meshCount - 1)]);
		}
	}
	
	//Setting values
	entities[0].GetTransform()->Rotate(0.3, 0.3, 0.1);
	entities[1].GetTransform()->MoveAbsolute(0.45f, 0.5f, 1.0);
	entities[2].GetTransform()->MoveAbsolute(0.0, 0.01, 1.0f);
	entities[3].GetTransform()->Scale(1.5f, 1.3f, 1.0f);
	//entities[4].GetTransform()->Rotate(0.45f, 0.5f, 1.3f);*/
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	
}

float color[4] = { 0.4f, 0.6f, 0.75f, 1.0f };//might have to delete it
// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	for (int i = 0; i < entities.size(); i++) {
		entities[i].GetTransform()->GetWorldMatrix();
	}

	FrameReset(deltaTime);
	BuildUI();
	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();

	//Animation
	float speed = 0.707f;
	float scale = (float)cos(totalTime) * 0.5f ;
	entities[3].GetTransform()->SetScale(scale, scale, scale);
	entities[0].GetTransform()->SetPosition((float)cos(totalTime * speed), (float)sin(totalTime * speed), 0);
	entities[4].GetTransform()->SetRotation(DirectX::XMFLOAT3(sin(totalTime) * speed, sin(totalTime) * speed, 0));
	
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	color);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	for(GameEntity& entity: entities)
	{
		ConstantBufferData cbData = {};
		cbData.worldMatrix = entity.GetTransform()->GetWorldMatrix();
		//TODO: Color Tint

		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		Graphics::Context->Map(constBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &cbData, sizeof(cbData));
		Graphics::Context->Unmap(constBuffer.Get(), 0);

		//Binds it in order to draw the mesh with cb data
		Graphics::Context->VSSetConstantBuffers(
			0,
			1,
			constBuffer.GetAddressOf());

		entity.GetMesh()->Draw();
	}
	
	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		ImGui::Render(); // Turns this frame’s UI into renderable triangles
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen

		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}

void Game::FrameReset(float deltaTime) {
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);
}

int count = 0;
void Game::BuildUI() {
	ImGui::Begin("Inspector");
	AppDetails();
	MeshDetails(entities[0].GetMesh(), "Triangle");
	MeshDetails(entities[1].GetMesh(), "Quad");
	MeshDetails(entities[2].GetMesh(), "Pentagon");
	MeshDetails(entities[3].GetMesh(), "Hexagon");

	for (unsigned int i = 0; i < entities.size(); i++)
	{
		std::shared_ptr<Transform> transform = entities[i].GetTransform();
		EntityValues(transform, i);
	}

	ImGui::End();
}
void Game::AppDetails() {
	if (ImGui::CollapsingHeader("App Details")) {
		static bool checked = false;
		static bool popUp = false;
		static float slide = 0.0f;
		ImGui::Text("Window Client Size: %dx%d", Window::Width(), Window::Height());
		ImGui::Text("Framerate: %f fps", ImGui::GetIO().Framerate);
		ImGui::ColorEdit4("Background Color", &color[0]);
		ImVec2 windowSize = ImGui::GetWindowSize();  // 👈 Get current ImGui window size
		ImGui::Text("ImGui Window Size: %.0fx%.0f", windowSize.x, windowSize.y);

		if (ImGui::Button(checked ? "Hide ImGui Demo Window" : "Show ImGui Demo Window")) {
			checked = !checked;
		}
		if (checked)
			ImGui::ShowDemoWindow();

		//1: checkbox
		ImGui::Checkbox("Another Window", &popUp);
		//2: graph
		float graph[100];
		for (int i = 0; i < 100; i++) {
			graph[i] = tanf(i + ImGui::GetTime() * 0.5f);
		}
		ImGui::PlotLines("Tan", graph, 100);
		//3: slide values
		ImGui::SliderFloat("float", &slide, 0.0f, 1.0f);
		//4.Draws a shape
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImU32 shapeColor = IM_COL32(255, 255, color[2], 255);
		ImVec2 min = ImVec2(windowSize.x / 2.0f, windowSize.y * 0.66f);
		drawList->AddCircle(min, 15.0f, shapeColor, 10, 5);
	}
}

void Game::MeshDetails(std::shared_ptr<Mesh> mesh, const char* name) {
	std::string title = std::string("Mesh: ") + name + std::string("##") + name;
	int triangles = mesh->GetIndexCount() / 3;
	if (ImGui::CollapsingHeader(title.c_str())) {
		ImGui::Text("Triangles: %i", triangles);
		ImGui::Text("Vertices : %i", mesh->GetVertexCount());
		ImGui::Text("Indices : %i", mesh->GetIndexCount());
	}
}

void Game::EntityValues(std::shared_ptr<Transform> entity, unsigned int i) {
	i += 1;
	std::string title = std::string("Entity ")+ std::to_string(i)+ 
		std::string("##") + std::to_string(i);

	if (ImGui::CollapsingHeader(title.c_str())) {

		//Save user input
		DirectX::XMFLOAT3 pos =  entity->GetPosition();
		DirectX::XMFLOAT3 rot = entity->GetPitchYawRoll();
		DirectX::XMFLOAT3 scale = entity->GetScale();
		
		std::string labelPos = "Position##" + std::to_string(i);
		std::string labelRot = "Rotation##" + std::to_string(i);
		std::string labelScale = "Scale##" + std::to_string(i);

		if(ImGui::DragFloat3(labelPos.c_str(), &pos.x))
			entity->SetPosition(pos);

		if(ImGui::DragFloat3(labelRot.c_str(), &rot.x))
			entity->SetRotation(rot);

		if(ImGui::DragFloat3(labelScale.c_str(), &scale.x))
			entity->SetScale(scale);

		entity->GetWorldMatrix();
		entity->GetWorldInverseTransposeMatrix();
	}
}

void Game::ConstantBufferUI(ConstantBufferData& cb) {
	if(ImGui::CollapsingHeader("CB Data")) {
		ImGui::SliderFloat4("Offset", &cb.worldMatrix._11, -1.0f, 1.0f);	
		ImGui::ColorEdit4("Color Tint", &cb.colorTint.x);
	}
}

