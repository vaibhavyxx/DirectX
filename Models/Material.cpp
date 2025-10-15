#include "Material.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <iostream>
#include "PathHelpers.h"
#include "Graphics.h"

void Material::LoadPixelShader()
{
	ID3DBlob* pixelShaderBlob;
	D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
	Graphics::Device->CreatePixelShader(
		pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize(),
		0,
		pixelShader.GetAddressOf()
	);
}

void Material::LoadVertexShader()
{
	ID3DBlob* vertexShaderBlob;
	D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);
	Graphics::Device->CreateVertexShader(
		vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize(),
		0,
		vertexShader.GetAddressOf()
	);
}

Material::Material()
{
	Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
	Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);

	Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
	Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);
}

void Material::SetColorTint(int r, int g, int b, int a)
{
}

void Material::SetColorTint(XMFLOAT4 color)
{
}

void Material::SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader)
{
}

void Material::SetPixelBuffer(Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader)
{
}
