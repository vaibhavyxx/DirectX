
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


Material::Material(std::shared_ptr<Shader> shader, DirectX::XMFLOAT4 color)
{
	this->shader = shader;
	this->vertexShader = shader->GetVertexShader();
	this->pixelShader = shader->GetPixelShader();
	this->colorTint = color;
	this->time = 0.0f;
}

void Material::SetColorTint(int r, int g, int b, int a)
{
	SetColorTint(XMFLOAT4(static_cast<float>(r), static_cast<float>(g), static_cast<float>(b), static_cast<float>(a)));
}

void Material::SetColorTint(XMFLOAT4 color)
{
	this->colorTint = color;
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

void Material::AddTextureSRV(unsigned int slot, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
	textureSRVs[slot] = srv;
}

void Material::AddSampler(unsigned int slot, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
	samplers[slot] = sampler;
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
