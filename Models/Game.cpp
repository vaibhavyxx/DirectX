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
#include "Camera.h"
#include "Shader.h"

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
	pixelShader = std::make_shared<Shader>();
	uvShader = std::make_shared<Shader>();
	normalShader = std::make_shared<Shader>();

	pixelShader->LoadVertexShader();
	pixelShader->LoadPixelShader("PixelShader.cso");

	uvShader->LoadVertexShader();
	uvShader->LoadPixelShader("DebugUVPS.cso");

	normalShader->LoadVertexShader();
	normalShader->LoadPixelShader("DebugNormalsPS.cso");

	Initialize();
	
	CreateGeometry();
	/*pixelShader->Setup();
	uvShader->Setup();
	normalShader->Setup();*/
	
}

void Game::Initialize() {

	currentCamera = 0;
	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();

	//Constant Buffer
	{
		unsigned int size = sizeof(ConstantBufferData);
		size = (size + 15) / 16 * 16;
	
		D3D11_BUFFER_DESC pixelDesc = {};
		pixelDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		pixelDesc.ByteWidth = size;
		pixelDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		pixelDesc.Usage = D3D11_USAGE_DYNAMIC;

		Graphics::Device->CreateBuffer(&pixelDesc, 0, constBuffer.GetAddressOf());
	}

	//Pixel Buffer
	{
		unsigned int size = sizeof(PixelStruct);
		size = (size + 15) / 16 * 16;

		D3D11_BUFFER_DESC cbDesc = {};
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.ByteWidth = size;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;

		Graphics::Device->CreateBuffer(&cbDesc, 0, pixelBuffer.GetAddressOf());
	}
	//float aspectRatio, DirectX::XMFLOAT3 initialPosition, 
	// float fieldOfView, float nearClip, float farClip, float movementSpeed, float mouseLookSpeed
	//Setting up a camera
	std::shared_ptr<Camera> cam1 = std::make_shared<Camera>(
		Window::AspectRatio(), 
		DirectX::XMFLOAT3(0.0f, 0.0f, -5.0f), 
		DirectX::XM_PIDIV4, 
		0.01, 1000, 1.0f, 0.01f, false
	);

	std::shared_ptr<Camera> cam2 = std::make_shared<Camera>(
		Window::AspectRatio(),
		DirectX::XMFLOAT3(0.0f,35.0f, -3.0f),
		1.0472f,	//60 degrees
		0.01, 1000, 5.0f, 0.5f, true
	);

	std::shared_ptr<Camera> cam3 = std::make_shared<Camera>(
		Window::AspectRatio(),
		DirectX::XMFLOAT3(3.0f, -5.0f, -5.0f),
		DirectX::XM_PIDIV2,
		0.01, 1000, 1.0f, 0.01f, true
	);

	cameras.push_back(cam1);
	cameras.push_back(cam2);
	cameras.push_back(cam3);
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
	/*pixelShader->LoadVertexShader();
	pixelShader->LoadPixelShader("PixelShader.cso");
	pixelShader->SetInputLayout();

	uvShader->LoadVertexShader();
	uvShader->LoadPixelShader("DebugUVPS.cso");
	uvShader->SetInputLayout();

	normalShader->LoadVertexShader();
	normalShader->LoadPixelShader("DebugNormalsPS.cso");
	normalShader->SetInputLayout();*/
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
	XMFLOAT4 white = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	std::shared_ptr<Material> red = std::make_shared<Material>(pixelShader->GetVertexShader(), pixelShader->GetPixelShader(), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	std::shared_ptr<Material> blueGreen = std::make_shared<Material>(pixelShader->GetVertexShader(), pixelShader->GetPixelShader(), DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));
	std::shared_ptr<Material> purple = std::make_shared<Material>(pixelShader->GetVertexShader(), pixelShader->GetPixelShader(), DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f));

	materials.push_back(red);
	materials.push_back(blueGreen);
	materials.push_back(purple);

	std::shared_ptr<Mesh> sphereModel = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/sphere.obj").c_str());
	std::shared_ptr<Mesh> cubeModel = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/cube.obj").c_str());
	entities.push_back(std::make_shared<GameEntity>(sphereModel,red, normalShader));
	entities.push_back(std::make_shared<GameEntity>(cubeModel, purple, normalShader));
	entities[0]->GetTransform()->SetPosition(-1.0f, 0.0f, -1.0f);
	entities[1]->GetTransform()->SetPosition(1.0f, 0.5f, 0.0f);

	pixelShader->LoadShaders();
	pixelShader->SetInputLayout();

	/*uvShader->LoadShaders();
	uvShader->SetInputLayout();*/
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	//Get window size
	for(int i =0; i < cameras.size(); i++)
		cameras[i]->UpdateProjectionMatrix(Window::AspectRatio());
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	FrameReset(deltaTime);
	BuildUI();
	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();

	//Animation
	float speed = 0.707f;
	float scale = (float)cos(totalTime) * 0.5f ;
	cameras[currentCamera]->Update(deltaTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(), color);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	for (std::shared_ptr<GameEntity> entity : entities)
	{
		entity->Draw(constBuffer, pixelBuffer, cameras[currentCamera]);
	}
	
	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Draw the UI after everything else
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
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

	for (unsigned int i = 0; i < entities.size(); i++)
	{
		std::shared_ptr<Transform> transform = entities[i]->GetTransform();
		EntityValues(transform, i);
	}

	if (ImGui::CollapsingHeader("Camera"))
	{
		DirectX::XMFLOAT4X4 view = cameras[currentCamera]->GetView();
		DirectX::XMFLOAT4X4 proj = cameras[currentCamera]->GetProjection();

		// Display view matrix
		ImGui::Text("View Matrix:");
		for (int i = 0; i < 4; i++)
		{
			ImGui::Text("%.2f %.2f %.2f %.2f",
				view.m[i][0], view.m[i][1], view.m[i][2], view.m[i][3]);
		}

		// Display projection matrix
		ImGui::Text("Projection Matrix:");
		for (int i = 0; i < 4; i++)
		{
			ImGui::Text("%.2f %.2f %.2f %.2f",
				proj.m[i][0], proj.m[i][1], proj.m[i][2], proj.m[i][3]);
		}
		DirectX::XMFLOAT3 pos = cameras[currentCamera]->GetTransform()->GetPosition();
		float fov = cameras[currentCamera]->GetFOV();
		bool orthographic = cameras[currentCamera]->IsOrthographic();

		ImGui::Text("FOV %.2f", fov);
		if (orthographic) ImGui::Text("Orthographic Camera");
		else ImGui::Text("Perspective Camera");

		if (ImGui::Button("Camera 1")) currentCamera = 0;
		if (ImGui::Button("Camera 2")) currentCamera = 1;
		if (ImGui::Button("Camera 3")) currentCamera = 2;
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
			graph[i] = static_cast<int>(tanf(i + ImGui::GetTime() * 0.5f));
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

void Game::EntityValues(std::shared_ptr<Transform> entity, unsigned int i)
{
	std::string title = "Entity " + std::to_string(i + 1) + "##" + std::to_string(i);

	if (ImGui::CollapsingHeader(title.c_str()))
	{
		DirectX::XMFLOAT3 pos = entity->GetPosition();
		DirectX::XMFLOAT3 rot = entity->GetPitchYawRoll();
		DirectX::XMFLOAT3 scale = entity->GetScale();

		std::string labelPos = "Position##" + std::to_string(i);
		std::string labelRot = "Rotation##" + std::to_string(i);
		std::string labelScale = "Scale##" + std::to_string(i);

		if (ImGui::DragFloat3(labelPos.c_str(), &pos.x, 0.01f, -1.0f, 1.0f))
			entity->SetPosition(pos);

		if (ImGui::DragFloat3(labelRot.c_str(), &rot.x, 0.01f, -XM_PI, XM_PI))
			entity->SetRotation(rot);

		if (ImGui::DragFloat3(labelScale.c_str(), &scale.x, 0.01f, 0.1f, 10.0f))
			entity->SetScale(scale);
	}
}




