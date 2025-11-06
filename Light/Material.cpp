
#include "Material.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <iostream>
#include "PathHelpers.h"
#include "Graphics.h"
#include "Camera.h"
#include "BufferStructs.h"
#include <memory>
#include "Transform.h"
#include "Shader.h"
#include <d3d11shadertracing.h>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <algorithm>
#include "Lights.h"
#include <vector>

Material::Material(std::shared_ptr<Shader> shader, DirectX::XMFLOAT4 color, float roughness, DirectX::XMFLOAT3 ambient)
{
	this->shader = shader;
	this->vertexShader = shader->GetVertexShader();
	this->pixelShader = shader->GetPixelShader();
	this->colorTint = color;
	this->time = 0.0f;
	this->scale = XMFLOAT2(1.0f, 1.0f);
	this->uvOffset = XMFLOAT2(0.0f, 0.0f);
	this->textureCounter = 0;
	this->samplerCounter = 0;
	this->roughness = roughness;
	this->ambient = ambient;
}

void Material::SetColorTint(int r, int g, int b, int a)
{
	SetColorTint(XMFLOAT4(static_cast<float>(r), static_cast<float>(g), static_cast<float>(b), static_cast<float>(a)));
}

void Material::SetColorTint(XMFLOAT4 color)
{
	this->colorTint = color;
}

void Material::SetRoughness(float value)
{
	this->roughness = std::clamp(value, 0.0f, 1.0f);
}

void Material::SetAmbient(float x, float y, float z)
{
	SetAmbient(XMFLOAT3(x, y, z));
}

void Material::SetAmbient(XMFLOAT3 value)
{
	this->ambient = value;
}

XMFLOAT3 Material::GetAmbient()
{
	return this->ambient;
}

void Material::SetupPixelStruct(std::shared_ptr<Camera> cam, Light* lights)
{
	PixelStruct pixelData = {};
	pixelData.colorTint = GetColorTint();
	pixelData.uvScale = GetScale();
	pixelData.offset = GetUVOffset();
	pixelData.time = GetTime();
	pixelData.roughness = GetRoughness();
	pixelData.camPos = cam->GetTransform()->GetPosition();
	pixelData.ambient = GetAmbient();

	pixelData.type = 0;
	pixelData.lightCount = 2;

	for (int i = 0; i < 5; i++) {
		pixelData.lights[i] = lights[i];
	}
	Graphics::FillAndBindNextCB(&pixelData, sizeof(PixelStruct), D3D11_PIXEL_SHADER, 0);
}

void Material::SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader)
{
	this->vertexShader = vertexShader;
}

void Material::SetPixelBuffer(Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader)
{
	this->pixelShader = pixelShader;
}

float Material::GetTime() {
	return time;
}

void Material::SetTime(float value)
{
	this->time = value;
}

XMFLOAT2 Material::GetUVOffset()
{
	return uvOffset;
}

void Material::SetUVOffset(XMFLOAT2 value)
{
	this->uvOffset = value;
}

void Material::AddTextureSRV(unsigned int slot, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
	textureSRVs[slot] = srv;
	textureCounter++;
}

void Material::AddSampler(unsigned int slot, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
	samplers[slot] = sampler;
	samplerCounter++;
}

void Material::BindTexturesAndSamplers()
{
	for (int i = 0; i < 128; i++) {
		Graphics::Context->PSSetShaderResources(i, 1, textureSRVs[i].GetAddressOf());
	}

	for (int i = 0; i < 16; i++) {
		Graphics::Context->PSSetSamplers(i, 1, samplers[i].GetAddressOf());
	}
}

XMFLOAT4 Material::GetColorTint()
{
	return colorTint;
}

float Material::GetRoughness()
{
	return this->roughness;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetShaderResourceView(int slot)
{
	if (textureSRVs[slot] == NULL)
		return NULL;

	return textureSRVs[slot];
}

std::shared_ptr<Shader> Material::GetShader()
{
	return this->shader;
}

int Material::GetTextureCounter()
{
	return textureCounter;
}

int Material::GetSamplerCounter()
{
	return samplerCounter;
}

XMFLOAT2 Material::GetScale() {
	return scale;
}
void Material::SetScale(XMFLOAT2 value) {
	scale = value;
}

void Material::SetScale(float x, float y)
{
	SetScale(XMFLOAT2(x, y));
}

