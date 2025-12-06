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
	LoadLights(0.5f);
	CreateTextures();

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	Graphics::Device->CreateSamplerState(&sampDesc, samplerState.GetAddressOf());
	Graphics::Device->CreateSamplerState(&sampDesc, samplerStateOverlay.GetAddressOf());

	shadowMapResolution = pow(2, 8);
	{
		D3D11_TEXTURE2D_DESC shadowDesc = {};
		shadowDesc.Width = shadowMapResolution;
		shadowDesc.Height = shadowMapResolution;
		shadowDesc.ArraySize = 1;
		shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
		shadowDesc.CPUAccessFlags = 0;
		shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;	//Reserves all 32 bits for a single value
		shadowDesc.MipLevels = 1;
		shadowDesc.MiscFlags = 0;
		shadowDesc.SampleDesc.Count = 1;
		shadowDesc.SampleDesc.Quality = 0;
		shadowDesc.Usage = D3D11_USAGE_DEFAULT;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTexture;
		Graphics::Device->CreateTexture2D(&shadowDesc, 0, shadowTexture.GetAddressOf());

		D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
		shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
		shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		shadowDSDesc.Texture2D.MipSlice = 0;
		Graphics::Device->CreateDepthStencilView(shadowTexture.Get(), &shadowDSDesc, shadowDSV.GetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = { };
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		Graphics::Device->CreateShaderResourceView(shadowTexture.Get(), &srvDesc, shadowSRV.GetAddressOf());
	}

	shader = std::make_shared<Shader>();
	skyShader = std::make_shared<Shader>();

	shader->LoadVertexShader("VertexShader.cso");
	shader->LoadPixelShader("PixelShader.cso");
	shader->CreatePixelBuffer();

	skyShader->LoadVertexShader("SkyVertexShader.cso");
	skyShader->LoadPixelShader("SkyPS.cso");
	skyShader->CreatePixelBuffer();

	//shadowShader->LoadVertexShader("ShadowMapVS.cso");
	{
		ID3DBlob* vertexShaderBlob;
		D3DReadFileToBlob(FixPath(L"ShadowMapVS.cso").c_str(), &vertexShaderBlob);

		Graphics::Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			shadowVertexShader.GetAddressOf());
	}
	Initialize();
	CreateGeometry();
}

void Game::Initialize() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
	ImGui::StyleColorsDark();
	LoadCameras();
}

Game::~Game()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Game::LoadLights(float offset)
{
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
			//dirLight.Direction = XMFLOAT3(offset * i, offset * i, 0.0f);
			dirLight.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
			dirLight.Intensity = 1.0f;
			dirLight.Range = 5.0f;
		}
		else if (i == 2) {
			dirLight.Type = LIGHT_TYPE_SPOT;
			dirLight.Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
			dirLight.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
			dirLight.Intensity = 0.5f;
			dirLight.Range = 2.0f;
			dirLight.Position = XMFLOAT3(offset * i, 0.0f, 0.0f);
			dirLight.SpotOuterAngle = XMConvertToRadians(60.0f);
			dirLight.SpotInnerAngle = XMConvertToRadians(45.0f);
		}
		else {
			dirLight.Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
			dirLight.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
			dirLight.Intensity = 1.0f;
			dirLight.Position = XMFLOAT3(offset * i, 0.0f, 0.0f);
		}
		dirLight.Intensity = 1.0f;
		lights[i] = dirLight;
	}
}

void Game::LoadCameras()
{
	currentCamera = 0;
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

	cameras = { cam1, cam2,cam3 };
}

void Game::CreateTextures()
{
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> color;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normal;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rough;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metal;

		LoadTextures("../../Assets/Materials/PBR/floor_albedo.png", color);
		LoadTextures("../../Assets/Materials/PBR/floor_normals.png", normal);
		LoadTextures("../../Assets/Materials/PBR/floor_roughness.png", rough);
		LoadTextures("../../Assets/Materials/PBR/floor_metal.png", metal);
		floorMaterials = { color, rough, normal, metal };
	}
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> color;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normal;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rough;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metal;

		LoadTextures("../../Assets/Materials/PBR/grass_albedo.png", color);
		LoadTextures("../../Assets/Materials/PBR/grass_normals.png", normal);
		LoadTextures("../../Assets/Materials/PBR/grass_roughness.png", rough);
		LoadTextures("../../Assets/Materials/PBR/grass_metal.png", metal);

		metalMaterials = { color, rough, normal, metal };
	}

	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> color;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normal;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rough;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metal;

		LoadTextures("../../Assets/Materials/PBR/cobblestone_albedo.png", color);
		LoadTextures("../../Assets/Materials/PBR/cobblestone_normals.png", normal);
		LoadTextures("../../Assets/Materials/PBR/cobblestone_roughness.png", rough);
		LoadTextures("../../Assets/Materials/PBR/cobblestone_metal.png", metal);
		cobblestoneMaterials = { color, rough, normal, metal };
	}

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
}

void Game::CreateMaterials()
{
	materials = {
		std::make_shared<Material>(shader, DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), 0.0f, ambientColor, floorMaterials[3], 0.0f, 1, 1, 1,1),
		std::make_shared<Material>(shader, DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), 0.5f, ambientColor, floorMaterials[3], 1.0f, 1,1, 1, 1),
		std::make_shared<Material>(shader, DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f), 0.25f, ambientColor, floorMaterials[3], 0.5f, 1,1,1,1) };

	materials[0]->AddTextureSRV(0, floorMaterials[0]);
	materials[0]->AddTextureSRV(1, floorMaterials[1]);
	materials[0]->AddTextureSRV(2, floorMaterials[2]);
	materials[0]->AddTextureSRV(3, floorMaterials[3]);
	materials[0]->AddSampler(0, samplerState);
	materials[0]->BindTexturesAndSamplers();

	materials[1]->AddTextureSRV(0, metalMaterials[0]);
	materials[1]->AddTextureSRV(1, metalMaterials[1]);
	materials[1]->AddTextureSRV(2, metalMaterials[2]);
	materials[1]->AddTextureSRV(3, metalMaterials[3]);
	materials[1]->AddSampler(0, samplerState);
	materials[1]->BindTexturesAndSamplers();

	materials[2]->AddTextureSRV(0, cobblestoneMaterials[0]);
	materials[2]->AddTextureSRV(1, cobblestoneMaterials[1]);
	materials[2]->AddTextureSRV(2, cobblestoneMaterials[2]);
	materials[2]->AddTextureSRV(3, cobblestoneMaterials[3]);
	materials[2]->AddSampler(0, samplerState);
	materials[2]->BindTexturesAndSamplers();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floor;
	{
		LoadTextures("../../Assets/Materials/PBR/wood_normals.png", floor);
	}
	floorMaterial = std::make_shared<Material>(shader, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, ambientColor, floor, 0.0f, 0, 0, 0, 0);
	floorMaterial->AddTextureSRV(0, floor);
	floorMaterial->AddSampler(0, samplerState);

}

// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	ambientColor = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	CreateMaterials();
	std::shared_ptr<Mesh> sphere = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/sphere.obj").c_str());
	std::shared_ptr<Mesh> cube = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/cube.obj").c_str());
	std::shared_ptr<Mesh> helix = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/helix.obj").c_str());
	std::shared_ptr<Mesh> cylinder = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/cylinder.obj").c_str());
	std::shared_ptr<Mesh> quad = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/quad.obj").c_str());
	std::shared_ptr<Mesh> quadDoubleSided = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/quad_double_sided.obj").c_str());
	std::shared_ptr<Mesh> torus = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/torus.obj").c_str());
	meshes = { sphere, cube, quad,cylinder, helix,quadDoubleSided, torus };

	sky = std::make_shared<Sky>(cube, samplerState, textures, skyShader);	//makes a sky
	floorGameObject = std::make_shared<GameEntity>(cube, floorMaterial);
	floorGameObject->GetTransform()->SetPosition(5.0f, -2.0f, 0.0f);
	floorGameObject->GetTransform()->SetScale(30.0f, 1.0f, 10.0f);

	float offset = 5.0f;
	for (int i = 0; i < meshes.size(); i++) {
		int index = i % materials.size();
		gameEntities.push_back(std::make_shared<GameEntity>(meshes[i], materials[index]));
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
	for (int i = 0; i < 5; i++) {
		switch (lights[i].Type) {
		case LIGHT_TYPE_DIRECTIONAL:
		{
			XMVECTOR dir = XMLoadFloat3(&lights[i].Direction);
			XMMATRIX lightView = XMMatrixLookToLH(
				dir * -20,
				dir,
				XMVectorSet(0, 1, 0, 0));

			float lightProjSize = 15.0f;
			XMMATRIX lightProj = XMMatrixOrthographicLH(
				lightProjSize, lightProjSize, 1.0f, 100.0f
			);
		}
		break;

		case LIGHT_TYPE_SPOT:
		{
			XMVECTOR dir = XMLoadFloat3(&lights[i].Direction);
			XMMATRIX lightView = XMMatrixLookToLH(
				XMLoadFloat3(&lights[i].Position),
				dir,
				XMVectorSet(0, 1, 0, 0));
			XMMATRIX lightProj = XMMatrixPerspectiveFovLH(
				lights[i].SpotOuterAngle, 1.0f, 1.0f, 100.0f
			);
		}

		break;

		case LIGHT_TYPE_POINT:
			break;
		}
	}

	FrameReset(deltaTime);

	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
	cameras[currentCamera]->Update(deltaTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	{
		Graphics::Context->ClearDepthStencilView(shadowDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		ID3D11RenderTargetView* nullRTV{};
		Graphics::Context->OMSetRenderTargets(1, &nullRTV, shadowDSV.Get());	//Ignores color output
		Graphics::Context->PSSetShader(0, 0, 0);	//disables PS for shadows
	}
	D3D11_VIEWPORT viewport = {};
	viewport.Width = (float)shadowMapResolution;
	viewport.Height = (float)shadowMapResolution;
	viewport.MaxDepth = 1.0f;
	Graphics::Context->RSSetViewports(1, &viewport);
	Graphics::Context->VSSetShader(shadowVertexShader.Get(), 0, 0);

	struct ShadowVSData
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 proj;
	};
	ShadowVSData shadowVSData = {};
	shadowVSData.view = lightViewMatrix;
	shadowVSData.proj = lightProjectionMatrix;
	// Loop and draw all entities
	for (auto& e : gameEntities)
	{
		shadowVSData.world = e->GetTransform()->GetWorldMatrix();
		Graphics::FillAndBindNextCB(&shadowVSData, sizeof(ShadowVSData), D3D11_VERTEX_SHADER, 0); 
		e->Draw(cameras[currentCamera], lights, ambientColor);
	}

	//Resets back to previous properties
	viewport.Width = 1280;
	viewport.Height = 720;
	{
		Graphics::Context->RSSetViewports(1, &viewport);
		Graphics::Context->OMSetRenderTargets( 1, Graphics::BackBufferRTV.GetAddressOf(), Graphics::DepthBufferDSV.Get());
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(), color);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}
	BuildUI();
	floorGameObject->Draw(cameras[currentCamera], &lights[0], ambientColor);
	for (int i = 0; i < gameEntities.size(); i++) {
		gameEntities[i]->Draw(cameras[currentCamera], &lights[0], ambientColor);
	}

	sky->Draw(deltaTime, cameras[currentCamera]);
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

void Game::LoadTextures(std::string filepath, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv)
{
	std::wstring wideFileName(filepath.begin(), filepath.end());
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(wideFileName).c_str(),
		0,
		srv.GetAddressOf());
}

void Game::MaterialsUI()
{
	//Will focus on this later
}




