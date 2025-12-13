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
#include "Lights.h"

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

// For the DirectX Math library
using namespace DirectX;

#define SHADER_TYPE_REGULAR		0;
#define SHADER_TYPE_POSTPROCESS  1;

// --------------------------------------------------------
// The constructor is called after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
Game::Game()
{
	applyBlur = true;
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

	D3D11_SAMPLER_DESC sampDescRamp = {};
	sampDescRamp.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDescRamp.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDescRamp.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDescRamp.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDescRamp.MaxAnisotropy = 16;
	sampDescRamp.MaxLOD = D3D11_FLOAT32_MAX;
	Graphics::Device->CreateSamplerState(&sampDescRamp, RampSampler.GetAddressOf());

	shader = std::make_shared<Shader>();
	skyShader = std::make_shared<Shader>();

	shader->LoadVertexShader("VertexShader.cso", Shader::ShaderType::Regular);
	shader->LoadPixelShader("RampPS.cso");
	shader->CreatePixelBuffer();

	skyShader->LoadVertexShader("SkyVertexShader.cso", Shader::ShaderType::Regular);
	skyShader->LoadPixelShader("SkyPS.cso");
	skyShader->CreatePixelBuffer();

	/*shader->LoadVertexShader("VertexShader.cso", Shader::ShaderType::Regular);
	shader->LoadPixelShader("PixelShader.cso");
	shader->CreatePixelBuffer();
	*/
	Initialize();
	CreateGeometry();
	CreateShadowResources();
	PostProcessSetup();
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

void Game::CreateShadowResources()
{
	Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Graphics::Context->VSSetShader(shadowVertexShader.Get(), 0, 0);

	shadowMapResolution = 1024;
	shadowProjection = 10.0f;

	shadowDSV.Reset();
	shadowSRV.Reset();
	shadowSampler.Reset();
	shadowRasterizer.Reset();

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

	//Comparision Shader
	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // COMPARISON filter!
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f;
	shadowSampDesc.BorderColor[1] = 1.0f;
	shadowSampDesc.BorderColor[2] = 1.0f;
	shadowSampDesc.BorderColor[3] = 1.0f;
	Graphics::Device->CreateSamplerState(&shadowSampDesc, &shadowSampler);

	//Set up a rasterizer
	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Multiplied by (smallest possible positive value storable in the depth buffer)
	shadowRastDesc.DepthBiasClamp = 0.0f;
	shadowRastDesc.SlopeScaledDepthBias = 1.0f;
	Graphics::Device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);

	D3D11_RASTERIZER_DESC shadowRastDescDepthBias = {};
	shadowRastDescDepthBias.FillMode = D3D11_FILL_SOLID;
	shadowRastDescDepthBias.CullMode = D3D11_CULL_BACK;
	shadowRastDescDepthBias.DepthClipEnable = false;
	shadowRastDescDepthBias.DepthBias = 10; // Multiplied by (smallest possible positive value storable in the depth buffer)
	shadowRastDescDepthBias.DepthBiasClamp = 0.0f;
	shadowRastDescDepthBias.SlopeScaledDepthBias = 1.0f;
	Graphics::Device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizerDepthBias);

	ID3DBlob* vertexShaderBlob;
	D3DReadFileToBlob(FixPath(L"ShadowMapVS.cso").c_str(), &vertexShaderBlob);

	Graphics::Device->CreateVertexShader(
		vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
		vertexShaderBlob->GetBufferSize(),		// How big is that data?
		0,										// No classes in this shader
		shadowVertexShader.GetAddressOf());

	const int size = 4;
	D3D11_INPUT_ELEMENT_DESC inputElements[size] = {};

	inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElements[0].SemanticName = "POSITION";
	inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

	inputElements[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElements[1].SemanticName = "TEXCOORD";
	inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

	inputElements[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElements[2].SemanticName = "NORMAL";
	inputElements[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

	inputElements[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElements[3].SemanticName = "TANGENT";
	inputElements[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

	Graphics::Device->CreateInputLayout(
		inputElements,
		size,
		vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize(),
		inputLayout.GetAddressOf());
	Graphics::Context->IASetInputLayout(inputLayout.Get());
}

void Game::DrawShadowData()
{
	ID3D11RenderTargetView* nullRTV{};
	Graphics::Context->OMSetRenderTargets(1, &nullRTV, shadowDSV.Get());	//Ignores color output
	Graphics::Context->ClearDepthStencilView(shadowDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	Graphics::Context->RSSetState(shadowRasterizer.Get());
	Graphics::Context->RSSetState(shadowRasterizerDepthBias.Get());

	D3D11_VIEWPORT viewport = {};
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.Width = (float)shadowMapResolution;
	viewport.Height = (float)shadowMapResolution;
	Graphics::Context->RSSetViewports(1, &viewport);
	Graphics::Context->VSSetShader(shadowVertexShader.Get(), 0, 0);

	//Singular loop
	for (int i = 0; i < 5; i++) {
		ShadowVSData vsData = {};
		vsData.view = lightViewMatrix[i];
		vsData.proj = lightProjectionMatrix[i];
		Graphics::Context->PSSetShader(0, 0, 0);

		for (auto& e : gameEntities)
		{
			vsData.world = e->GetTransform()->GetWorldMatrix();
			Graphics::FillAndBindNextCB(&vsData, sizeof(ShadowVSData), D3D11_VERTEX_SHADER, 0);
			e->GetMesh()->Draw();
		}
		vsData.world = floorGameObject->GetTransform()->GetWorldMatrix();
		Graphics::FillAndBindNextCB(&vsData, sizeof(ShadowVSData), D3D11_VERTEX_SHADER, 0);
		floorGameObject->GetMesh()->Draw();

		for (auto& e : lightObjects)
		{
			vsData.world = e->GetTransform()->GetWorldMatrix();
			Graphics::FillAndBindNextCB(&vsData, sizeof(ShadowVSData), D3D11_VERTEX_SHADER, 0);
			e->GetMesh()->Draw();
		}
	}

	Graphics::Context->OMSetRenderTargets(1, Graphics::BackBufferRTV.GetAddressOf(), Graphics::DepthBufferDSV.Get());
	viewport.Width = (float)Window::Width();
	viewport.Height = (float)Window::Height();

	Graphics::Context->RSSetViewports(1, &viewport);
	Graphics::Context->RSSetState(0);
}

void Game::LoadLights(float offset)
{
	Light dir = {};
	dir.Type = LIGHT_TYPE_DIRECTIONAL;
	dir.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
	dir.Direction = XMFLOAT3(1.0f, -0.5f, 0.8f);
	dir.Intensity = 1.0f;

	Light spot = {};
	spot.Type = LIGHT_TYPE_SPOT;
	spot.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
	spot.Direction = XMFLOAT3(-0.1f, -0.4f, 1.0f);
	spot.Position = XMFLOAT3(4.0f, 0.0f, 0.0f);
	spot.Intensity = 1.0f;
	spot.Range = 50.0f;
	spot.SpotOuterAngle = XMConvertToRadians(80.0f);
	spot.SpotInnerAngle = XMConvertToRadians(60.0f);

	Light anotherDir = dir;
	anotherDir.Direction = XMFLOAT3(-1.0f, 1.0f, 0.8f);
	//anotherDir.Intensity = 1.0f;
	anotherDir.Color = XMFLOAT3(1.0f, 1.0f, 0.5f);

	Light oneMoreDir = dir;
	oneMoreDir.Direction = XMFLOAT3(1.0f, -0.5f, 0.8f);
	oneMoreDir.Intensity = 1.0f;
	oneMoreDir.Color = XMFLOAT3(1.0f, 1.0f, 0.5f);

	Light anotherSpot = spot;
	anotherSpot.Position = XMFLOAT3(12.77f, -1.39f, -1.41f);
	anotherSpot.Direction = XMFLOAT3(-0.3f, -1.0f, -1.0f);

	Light copySpot = spot;
	copySpot.Direction = XMFLOAT3(1.0f, 1.0f, 1.0f);
	copySpot.Position = XMFLOAT3(16.49f, 6.09f, -9.77f);
	copySpot.Range = 100.0f;

	lights[0] = dir;
	lights[1] = dir;
	lights[2] = dir;
	lights[3] = anotherDir;
	lights[4] = dir;
	//lights[5] = dir;
	//lights[6] = dir;
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
	LoadTextures("../../Assets/Materials/Ramps/toonRamp.png", rampTexture);

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
	materials[0]->AddTextureSRV(4, shadowSRV);
	materials[0]->AddSampler(0, samplerState);
	materials[0]->AddSampler(1, RampSampler);
	materials[0]->BindTexturesAndSamplers();

	materials[1]->AddTextureSRV(0, metalMaterials[0]);
	materials[1]->AddTextureSRV(1, metalMaterials[1]);
	materials[1]->AddTextureSRV(2, metalMaterials[2]);
	materials[1]->AddTextureSRV(3, metalMaterials[3]);
	materials[1]->AddTextureSRV(4, shadowSRV);
	materials[1]->AddSampler(0, samplerState);
	materials[1]->AddSampler(1, RampSampler);
	materials[1]->BindTexturesAndSamplers();

	materials[2]->AddTextureSRV(0, cobblestoneMaterials[0]);
	materials[2]->AddTextureSRV(1, cobblestoneMaterials[1]);
	materials[2]->AddTextureSRV(2, cobblestoneMaterials[2]);
	materials[2]->AddTextureSRV(3, cobblestoneMaterials[3]);
	materials[2]->AddTextureSRV(4, shadowSRV);
	materials[2]->AddSampler(0, samplerState);
	materials[2]->AddSampler(1, RampSampler);
	materials[2]->BindTexturesAndSamplers();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floor;
	{
		LoadTextures("../../Assets/Materials/PBR/wood_normals.png", floor);
	}
	floorMaterial = std::make_shared<Material>(shader, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, ambientColor, floor, 0.0f, 0, 0, 0, 0);
	floorMaterial->AddTextureSRV(0, floor);
	floorMaterial->AddSampler(0, samplerState);
	floorMaterial->AddSampler(1, RampSampler);
	floorMaterial->AddTextureSRV(4, rampTexture);
	
}

std::shared_ptr<GameEntity> lightEntity;
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

	std::shared_ptr<Mesh> lightMesh = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/sphere.obj").c_str());
	for (int i = 0; i < 5; i++) {
		XMFLOAT3 color = XMFLOAT3(1, 1, 1);//lights[i].Color;
		std::shared_ptr<Material> lightMaterial = std::make_shared<Material>(shader, DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f), 0.0f, ambientColor, floorMaterials[3], 0.0f, 0, 0, 0, 0);
		std::shared_ptr<GameEntity> lightEntity = std::make_shared<GameEntity>(lightMesh, lightMaterial);
		XMFLOAT3 pos;
		if (lights[i].Type == LIGHT_TYPE_DIRECTIONAL)
			pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		else
			pos = lights[i].Position;

		lightEntity->GetTransform()->SetPosition(pos);
		lightEntity->GetTransform()->SetScale(0.5f, 0.5, 0.5f);
		lightObjects.push_back(lightEntity);

		if (i == 4) {
			floorGameObject = std::make_shared<GameEntity>(cube, lightMaterial);
			floorGameObject->GetTransform()->SetPosition(0.0f, -3.0f, 0.0f);
			floorGameObject->GetTransform()->SetScale(30.0f, 1.0f, 30.0f);
		}
	}
	sky = std::make_shared<Sky>(cube, samplerState, textures, skyShader);	//makes a sky
	float offset = 3.5f;
	for (int i = 0; i < meshes.size(); i++) {
		int index = i % materials.size();
		gameEntities.push_back(std::make_shared<GameEntity>(meshes[i], materials[index]));
		gameEntities[i]->GetTransform()->SetPosition(0.0f, 0.0f, offset * i);
		//gameEntities[i]->GetTransform()->SetPosition(offset * i, 0.0f, 0.0f);
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
float dist = 0;
float threshold = 0.005f;
float speed = 0.001f;
//float angleOffset = 0.707f;
void Game::Update(float deltaTime, float totalTime)
{
	for (int i = 0; i < lightObjects.size(); i++) {
		if (lights[i].Type == LIGHT_TYPE_DIRECTIONAL) continue;
		lights[i].Position = (lightObjects[i]->GetTransform()->GetPosition());
	}

	int oldShadowRes = shadowMapResolution;
	if (oldShadowRes != shadowMapResolution)
		CreateShadowResources();

	for (int i = 0; i < 1; i++) {	//testing
		switch (lights[i].Type) {
		case LIGHT_TYPE_DIRECTIONAL:
		{
			XMVECTOR dir = XMLoadFloat3(&lights[i].Direction);
			XMMATRIX lightView = XMMatrixLookToLH(
				dir * -20,
				dir,
				XMVectorSet(0, 1, 0, 0));
			XMStoreFloat4x4(&lightViewMatrix[i], lightView);

			float lightProjSize = 15.0f;
			XMMATRIX lightProj = XMMatrixOrthographicLH(
				lightProjSize, lightProjSize, 1.0f, 100.0f
			);
			XMStoreFloat4x4(&lightProjectionMatrix[i], lightProj);
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
			XMStoreFloat4x4(&lightViewMatrix[i], lightView);
			XMStoreFloat4x4(&lightProjectionMatrix[i], lightProj);
		}

		break;

		case LIGHT_TYPE_POINT:
			break;
		}
	}
	FrameReset(deltaTime);

	for (int i = 0; i < gameEntities.size(); i++) {
		dist += speed * deltaTime;
		XMFLOAT3 pos = gameEntities[i]->GetTransform()->GetPosition();
		XMFLOAT3 newPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		if (i % 2 == 0) newPos = XMFLOAT3(dist + pos.x, pos.y, pos.z);
		else newPos = XMFLOAT3(pos.x, pos.y, pos.z + dist);
		gameEntities[i]->GetTransform()->SetPosition(newPos);

		if (abs(dist) > threshold) {
			dist = 0.0f;
			speed *= -1.0f;
		}
	}
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
	cameras[currentCamera]->Update(deltaTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(), color);
	Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	DrawShadowData();

	const float rtClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	Graphics::Context->ClearRenderTargetView(postProcessRTV.Get(), rtClearColor);
	Graphics::Context->OMSetRenderTargets(1, postProcessRTV.GetAddressOf(), Graphics::DepthBufferDSV.Get());

	for (int k = 0; k < 1; k++) {
		for (int i = 0; i < gameEntities.size(); i++) {
			gameEntities[i]->GetMaterial()->AddSampler(0, samplerState);
			gameEntities[i]->GetMaterial()->AddSampler(1, RampSampler);
			gameEntities[i]->GetMaterial()->AddTextureSRV(4, rampTexture);
			//gameEntities[i]->GetMaterial()->AddTextureSRV(5, rampTexture);

			gameEntities[i]->Draw(cameras[currentCamera], &lights[0], ambientColor, lightViewMatrix[k], lightProjectionMatrix[k]);
		}

		floorGameObject->GetMaterial()->AddSampler(0, samplerState);
		floorGameObject->GetMaterial()->AddSampler(1, RampSampler);
		floorGameObject->GetMaterial()->AddTextureSRV(4, rampTexture);
		//floorGameObject->GetMaterial()->AddTextureSRV(5, rampTexture);

		floorGameObject->Draw(cameras[currentCamera], &lights[0], ambientColor, lightViewMatrix[k], lightProjectionMatrix[k]);
		sky->Draw(deltaTime, cameras[currentCamera]);

		for (int i = 0; i < 5; i++) {
			lightObjects[i]->GetMaterial()->AddSampler(0, samplerState);
			lightObjects[i]->GetMaterial()->AddSampler(1, RampSampler);
			lightObjects[i]->GetMaterial()->AddTextureSRV(4, rampTexture);
			//gameEntities[i]->GetMaterial()->AddTextureSRV(5, rampTexture);
			lightObjects[i]->Draw(cameras[currentCamera], lights, ambientColor, lightViewMatrix[k], lightProjectionMatrix[k]);
		}
	}

	Graphics::Context->OMSetRenderTargets(1, Graphics::BackBufferRTV.GetAddressOf(), 0);
	if (applyBlur) {
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		ID3D11Buffer* nothing = 0;
		Graphics::Context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);
		Graphics::Context->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);

		//setting shaders
		Graphics::Context->VSSetShader(postProcessShader->GetVertexShader().Get(), 0, 0);
		Graphics::Context->PSSetShader(postProcessShader->GetPixelShader().Get(), 0, 0);
		Graphics::Context->PSSetShaderResources(0, 1, postProcessSRV.GetAddressOf());
		Graphics::Context->PSSetSamplers(0, 1, postProcessSampler.GetAddressOf());

		BlurData data = {};
		data.blurRadius = blurRadius;
		data.pixelWidth = blurAmount;
		data.pixelHeight = blurAmount;
		Graphics::FillAndBindNextCB(&data, sizeof(BlurData), D3D11_PIXEL_SHADER, 0);
	}
	Graphics::Context->Draw(3, 0);
	ID3D11ShaderResourceView* nullSRVs[16] = {};
	Graphics::Context->PSSetShaderResources(0, 16, nullSRVs);

	//POST RENDER
	BuildUI();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Present at the end of the frame
	bool vsync = Graphics::VsyncState();
	Graphics::SwapChain->Present(
		vsync ? 1 : 0,
		vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

	Graphics::Context->OMSetRenderTargets(1, postProcessRTV.GetAddressOf(), Graphics::DepthBufferDSV.Get());
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
		EntityValues(gameEntities[i], i, "Entities");
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
			std::string heading = "Light No##" + std::to_string(i);
			if (ImGui::CollapsingHeader(heading.c_str()))
			{
				DirectX::XMFLOAT3 colorValue = lights[i].Color;
				float intensityValue = lights[i].Intensity;
				DirectX::XMFLOAT3 dir = lights[i].Direction;
				float range = lights[i].Range;

				std::string label = "Light##" + std::to_string(i);
				std::string rangeLabel = "Range##" + std::to_string(i);
				std::string color = "Color##" + std::to_string(i);
				std::string intensity = "Intensity##" + std::to_string(i);
				std::string direction = "Direction##" + std::to_string(i);

				if (ImGui::DragFloat3(direction.c_str(), &dir.x, 0.1f, -1.0f, 1.0f))
					lights[i].Direction = dir;

				if (ImGui::ColorEdit3(color.c_str(), &colorValue.x))
					lights[i].Color = colorValue;

				if (ImGui::DragFloat(intensity.c_str(), &intensityValue, 0.01f, 0.0f, 1.0f))
					lights[i].Intensity = intensityValue;

				if (ImGui::DragFloat(rangeLabel.c_str(), &range, 0.01f, 0.0f, 1.0f))
					lights[i].Range = range;

				EntityValues(lightObjects[i], i, "Light's Transform Values");
			}
		}
	}

	if (ImGui::CollapsingHeader("Shadows")) {
		ImGui::Image(shadowSRV.Get(), ImVec2(512, 512));
	}

	if (ImGui::CollapsingHeader("Post Processing")) {
		std::string labelImplement = "Apply Blur";
		std::string labelBlur = "Blur Amount";
		std::string labelBlurRadius = "Blur Radius";

		if (ImGui::Checkbox(labelImplement.c_str(), &applyBlur)) applyBlur;
		if (ImGui::DragFloat(labelBlur.c_str(), &blurAmount, 0.001f, 0.0f, 0.01f)) blurAmount;
		if (ImGui::DragInt(labelBlurRadius.c_str(), &blurRadius, 0, 1, 10)) blurRadius;
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

void Game::EntityValues(std::shared_ptr<GameEntity> entity, unsigned int i, std::string title)
{
	std::string header = title + std::to_string(i + 1) + "##" + std::to_string(i);

	if (ImGui::CollapsingHeader(header.c_str()))
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

		if (ImGui::DragFloat3(labelPos.c_str(), &pos.x, 0.01f, -50.0f, 50.0f))
			entity->GetTransform()->SetPosition(pos);

		if (ImGui::DragFloat3(labelRot.c_str(), &rot.x, 0.01f, -XM_PI, XM_PI))
			entity->GetTransform()->SetRotation(rot);

		if (ImGui::DragFloat3(labelScale.c_str(), &scale.x, 0.01f, 0.1f, 20.0f))
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

void Game::PostProcessSetup()
{
	postProcessShader = std::make_shared<Shader>();
	postProcessShader->LoadVertexShader("PostProcessVS.cso", Shader::ShaderType::Regular);
	postProcessShader->LoadPixelShader("PostProcessPS.cso");
	postProcessShader->CreatePixelBuffer();

	{
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Graphics::Context->VSSetShader(postProcessShader->GetVertexShader().Get(), 0, 0);
		Graphics::Context->PSSetShader(postProcessShader->GetPixelShader().Get(), 0, 0);
	}

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = (unsigned int)(Window::Width());
	textureDesc.Height = (unsigned int)(Window::Height());
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // Will render to it and sample from it!
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> ppTexture;
	Graphics::Device->CreateTexture2D(&textureDesc, 0, ppTexture.GetAddressOf());

	// Create the Render Target View
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	Graphics::Device->CreateRenderTargetView(ppTexture.Get(), &rtvDesc, postProcessRTV.ReleaseAndGetAddressOf());
	Graphics::Device->CreateShaderResourceView( ppTexture.Get(), 0, postProcessSRV.ReleaseAndGetAddressOf());

	// Sampler state for post processing
	D3D11_SAMPLER_DESC ppSampDesc = {};
	ppSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	ppSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	ppSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	ppSampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	ppSampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	Graphics::Device->CreateSamplerState(&ppSampDesc, postProcessSampler.GetAddressOf());
}