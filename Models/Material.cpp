
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

Material::Material(Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader,
Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader, DirectX::XMFLOAT4 color)
{
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
	this->colorTint = color;
}

void Material::SetColorTint(int r, int g, int b, int a)
{
	SetColorTint(XMFLOAT4(r,g,b,a));
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

void Material::MaterialSetup(Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer, 
	std::shared_ptr<Transform> transform, std::shared_ptr<Camera> cam)
{
	ConstantBufferData vsData = {};
	vsData.worldMatrix = transform->GetWorldMatrix();
	vsData.viewMatrix = cam->GetView();
	vsData.projectionMatrix = cam->GetProjection();
	//vsData.colorTint = colorTint;

	// Copy this data to the constant buffer we intend to use
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	Graphics::Context->Map(vsConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

	// Straight memcpy() into the resource
	memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));

	// Unmap so the GPU can once again use the buffer
	Graphics::Context->Unmap(vsConstantBuffer.Get(), 0);
	Graphics::Context->VSSetConstantBuffers(0, 1, vsConstantBuffer.GetAddressOf());
}

void Material::SetConstantBuffer()
{
	
}
