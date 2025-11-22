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
#include "WICTextureLoader.h"

// This code assumes files are in "ImGui" subfolder!
// Adjust as necessary for your own folder structure and project setup
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "Lights.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// The constructor is called after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
Game::Game()
{
	float offset = 0.5f;
	for (int i = 0; i < 5; i++) {
		Light dirLight = {};
		dirLight.Type = LIGHT_TYPE_DIRECTIONAL;
		if (i == 1) {
			dirLight.Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
			dirLight.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
			dirLight.Intensity = 1.0f;
		}
		else if (i == 0) {
			dirLight.Type = LIGHT_TYPE_POINT;
			dirLight.Position = XMFLOAT3(5.0f, 1.0f, 0.0f);
			dirLight.Direction = XMFLOAT3(offset * i, offset * i, 0.0f);
			dirLight.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
			dirLight.Intensity = 1.0f;
			dirLight.Range = 0.5f;
		}
		else if (i == 2) {
			dirLight.Type = LIGHT_TYPE_SPOT;
			dirLight.Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
			dirLight.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
			dirLight.Intensity = 0.5f;
			dirLight.Range = 0.5f;
			dirLight.Position = XMFLOAT3(offset * i, 0.0f, 0.0f);
			dirLight.SpotOuterAngle = XMConvertToRadians(60.0f);
			dirLight.SpotInnerAngle = XMConvertToRadians(45.0f);
		}
		else{
			dirLight.Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
			dirLight.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
			dirLight.Intensity = 1.0f;
			dirLight.Position = XMFLOAT3(offset * i, 0.0f, 0.0f);
		}
		
		lights[i] = dirLight;
	}

	//Loads textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobblestone;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cushion;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rock;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobblestoneNRM;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cushionNRM;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> flatNRM;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rockNRM;

#pragma region Texture files
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Materials/cobblestone.png").c_str(),
		0,
		cobblestone.GetAddressOf());

	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Materials/cushion.png").c_str(),
		0,
		cushion.GetAddressOf());

	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Materials/rock.png").c_str(),
		0,
		rock.GetAddressOf());

#pragma endregion

#pragma region Normals
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Materials/cobblestone_normals.png").c_str(),
		0,
		cobblestoneNRM.GetAddressOf());

	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Materials/cushion_normals.png").c_str(),
		0,
		cushionNRM.GetAddressOf());

	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Materials/flat_normals.png").c_str(),
		0,
		flatNRM.GetAddressOf());

	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Materials/rock_normals.png").c_str(),
		0,
		rockNRM.GetAddressOf());
#pragma endregion

#pragma region Sky
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> back;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> down;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> front;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> left;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> right;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> up;

	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Clouds/back.png").c_str(),
		(ID3D11Resource**)textures[5].GetAddressOf(),
		back.GetAddressOf());

	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Clouds/down.png").c_str(),
		(ID3D11Resource**)textures[3].GetAddressOf(),
		down.GetAddressOf());

	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Clouds/front.png").c_str(),
		(ID3D11Resource**)textures[4].GetAddressOf(),
		front.GetAddressOf());

	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Clouds/left.png").c_str(),
		(ID3D11Resource**)textures[1].GetAddressOf(),
		left.GetAddressOf());

	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Clouds/right.png").c_str(),
		(ID3D11Resource**)textures[0].GetAddressOf(),
		right.GetAddressOf());

	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Clouds/up.png").c_str(),
		(ID3D11Resource**)textures[2].GetAddressOf(),
		up.GetAddressOf());
#pragma endregion

	srvVector = { cobblestone, cushion, rock };
	normalsSRV = { cobblestoneNRM, cushionNRM, rockNRM, flatNRM };
	//skySRV = { back, down, front, left, right, up };

	//Calls PS Set Shader Resources
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	Graphics::Device->CreateSamplerState(&sampDesc, samplerState.GetAddressOf());
	Graphics::Device->CreateSamplerState(&sampDesc, samplerStateOverlay.GetAddressOf());

	shader = std::make_shared<Shader>();
	skyShader = std::make_shared<Shader>();

	shader->LoadVertexShader("VertexShader.cso");
	shader->LoadPixelShader("PixelShader.cso");
	shader->CreatePixelBuffer();

	skyShader->LoadVertexShader("SkyVertexShader.cso");
	skyShader->LoadPixelShader("SkyPS.cso");
	skyShader->CreatePixelBuffer();

	Initialize();
	CreateGeometry();
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

	//Setting up a camera
	std::shared_ptr<Camera> cam1 = std::make_shared<Camera>(
		Window::AspectRatio(),
		DirectX::XMFLOAT3(0.0f, 0.0f, -5.0f),
		DirectX::XM_PIDIV4,
		0.01f, 1000.0f, 1.0f, 0.01f, 200.0f, false
	);

	std::shared_ptr<Camera> cam2 = std::make_shared<Camera>(
		Window::AspectRatio(),
		DirectX::XMFLOAT3(0.0f, 0.0f, -5.0f),
		DirectX::XM_PIDIV4,	//60 degrees
		0.01f, 1000.0f, 5.0f, 0.5f, 100.0f, true
	);

	std::shared_ptr<Camera> cam3 = std::make_shared<Camera>(
		Window::AspectRatio(),
		DirectX::XMFLOAT3(3.0f, -5.0f, -5.0f),
		DirectX::XM_PIDIV2,
		0.01f, 1000.0f, 1.0f, 0.01f, 200.0f, false
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
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	ambientColor = DirectX::XMFLOAT3(0.545f, 0.74f, 0.97f);
	materials = { 
		std::make_shared<Material>(shader, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, ambientColor, normalsSRV[3], false),
		std::make_shared<Material>(shader, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, ambientColor, normalsSRV[3], false),
		std::make_shared<Material>(shader, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.25f, ambientColor, normalsSRV[3], false)};

	for (int i = 0; i < 3; i++) {
		materials[i]->AddTextureSRV(0, srvVector[i]);
		materials[i]->AddTextureSRV(2, normalsSRV[i]);
		//materials[i]->SetColorTint(DirectX::XMFLOAT3(1.0f, 0.f, 1.0f));
		//materials[i]->AddTextureSRV(1, srvOverlay);		//additional texture for combine.cso
		materials[i]->AddSampler(0, samplerState);
		//materials[i]->AddSampler(1, samplerState);
		materials[i]->BindTexturesAndSamplers();
	}

	std::shared_ptr<Mesh> sphere = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/sphere.obj").c_str());
	std::shared_ptr<Mesh> cube = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/cube.obj").c_str());
	std::shared_ptr<Mesh> helix = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/helix.obj").c_str());
	std::shared_ptr<Mesh> cylinder = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/cylinder.obj").c_str());
	std::shared_ptr<Mesh> quad = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/quad.obj").c_str());
	std::shared_ptr<Mesh> quadDoubleSided = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/quad_double_sided.obj").c_str());
	std::shared_ptr<Mesh> torus = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/torus.obj").c_str());
	meshes = {sphere, cube, quad,cylinder, helix,quadDoubleSided, torus };

	sky = std::make_shared<Sky>(cube, samplerState, textures, skyShader);	//makes a sky

	float offset = 5.0f;

	for (int i = 0; i < meshes.size(); i++) {
		int materialsCount = i % materials.size();
		gameEntities.push_back(std::make_shared<GameEntity>(meshes[i], materials[materialsCount]));
		gameEntities[i]->GetTransform()->SetPosition(offset * i, 0.0f, 0.0f);
	}
}
// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	//Get window size
	for (int i = 0; i < cameras.size(); i++)
		cameras[i]->UpdateProjectionMatrix(Window::AspectRatio());
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
float d = 0;
//float angleOffset = 0.707f;
void Game::Update(float deltaTime, float totalTime)
{
	//light.Color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	FrameReset(deltaTime);

	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
	float speed = 0.5f;
	d += speed * deltaTime;
	for (int i = 0; i < gameEntities.size(); i++) {
		//gameEntities[i]->GetTransform()->SetRotation(d, d, 1.0f);
	}
	cameras[currentCamera]->Update(deltaTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		//Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(), color);
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(), color);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	}
	

	BuildUI();
	for (int i = 0; i < gameEntities.size(); i++) {
		gameEntities[i]->Draw(cameras[currentCamera], &lights[0], ambientColor);
	}

	sky->Draw(deltaTime, cameras[currentCamera]);
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

	for (unsigned int i = 0; i < gameEntities.size(); i++)
	{
		EntityValues(gameEntities[i], i);
	}

	if (ImGui::CollapsingHeader("Camera"))
	{
		DirectX::XMFLOAT4X4 view = cameras[currentCamera]->GetView();
		DirectX::XMFLOAT4X4 proj = cameras[currentCamera]->GetProjection();

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
	if (ImGui::CollapsingHeader("Lights")) {
		for (int i = 0; i < 5; i++) {
			DirectX::XMFLOAT3 colorValue = lights[i].Color;
			float intensityValue = lights[i].Intensity;

			std::string label = "Light##" + std::to_string(i);
			std::string color = "Color##" + std::to_string(i);
			std::string intensity = "Intensity##" + std::to_string(i);

			if (ImGui::ColorEdit3(color.c_str(), &colorValue.x))
				lights[i].Color = colorValue;

			if (ImGui::DragFloat(intensity.c_str(), &intensityValue, 0.01f, 0.0f, 1.0f))
				lights[i].Intensity = intensityValue;
		}
		std::string ambientLabel = "Ambience";
		XMFLOAT3 colorRGB = ambientColor;
		if (ImGui::ColorEdit3(ambientLabel.c_str(), &colorRGB.x)) {
		
			ambientColor = colorRGB;
		}
			
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
			graph[i] = (tanf(i + static_cast<float>(ImGui::GetTime()) * 0.5f));
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

void Game::EntityValues(std::shared_ptr<GameEntity> entity, unsigned int i)
{
	std::string title = "Entity " + std::to_string(i + 1) + "##" + std::to_string(i);

	if (ImGui::CollapsingHeader(title.c_str()))
	{
		DirectX::XMFLOAT3 pos = entity->GetTransform()->GetPosition();
		DirectX::XMFLOAT3 rot = entity->GetTransform()->GetPitchYawRoll();
		DirectX::XMFLOAT3 scale = entity->GetTransform()->GetScale();
		DirectX::XMFLOAT2 offset = entity->GetMaterial()->GetUVOffset();
		DirectX::XMFLOAT2 matScale = entity->GetMaterial()->GetScale();

		std::string labelPos = "Position##" + std::to_string(i);
		std::string labelRot = "Rotation##" + std::to_string(i);
		std::string labelScale = "Scale##" + std::to_string(i);
		std::string labelOffset = "Offset##" + std::to_string(i);
		std::string labelMatScale = "Material Scale##" + std::to_string(i);

		if (ImGui::DragFloat3(labelPos.c_str(), &pos.x, 0.01f, -1.0f, 1.0f))
			entity->GetTransform()->SetPosition(pos);

		if (ImGui::DragFloat3(labelRot.c_str(), &rot.x, 0.01f, -XM_PI, XM_PI))
			entity->GetTransform()->SetRotation(rot);

		if (ImGui::DragFloat3(labelScale.c_str(), &scale.x, 0.01f, 0.1f, 10.0f))
			entity->GetTransform()->SetScale(scale);

		std::shared_ptr<Material> mat = entity->GetMaterial();

		std::string imageTitle = "Color Tint " + std::to_string(i + 1) + "##" + std::to_string(i);
		XMFLOAT4 tint = mat->GetColorTint();
		if (ImGui::ColorEdit4(imageTitle.c_str(), &tint.x))
			entity->GetMaterial()->SetColorTint(tint);

		if (ImGui::DragFloat2(labelOffset.c_str(), &offset.x, 0.01f, -1.0f, 1.0f))
			entity->GetMaterial()->SetUVOffset(offset);

		if (ImGui::DragFloat2(labelMatScale.c_str(), &matScale.x, 0.01f, 0.0f, 3.0f))
			entity->GetMaterial()->SetScale(matScale);

		void* matSRV = mat->GetShaderResourceView(0).Get();
		ImGui::Image(matSRV, ImVec2(50.0f, 50.0f));

		if (i >= 2 && i <= 4) {
			void* matSRV2 = mat->GetShaderResourceView(1).Get();
			ImGui::Image(matSRV2, ImVec2(50.0f, 50.0f));
		}
	}
}




