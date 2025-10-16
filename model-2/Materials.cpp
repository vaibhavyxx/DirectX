#include "Materials.h"
#include "PathHelpers.h"
#include <d3dcompiler.h>
#include "Graphics.h"

Materials::Materials()
{
}

DirectX::XMFLOAT4 Materials::GetColorTint()
{
	return colorTint;
}

Microsoft::WRL::ComPtr<ID3D11VertexShader> Materials::GetVertexShader()
{
	return vertexShader;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> Materials::GetPixelShader()
{
	return pixelShader;
}

void Materials::LoadVertexShader()
{
	D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

	Graphics::Device->CreateVertexShader(
		vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
		vertexShaderBlob->GetBufferSize(),		// How big is that data?
		0,										// No classes in this shader
		vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
}

void Materials::LoadPixelShader()
{
	D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
	Graphics::Device->CreatePixelShader(
		pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
		pixelShaderBlob->GetBufferSize(),		// How big is that data?
		0,										// No classes in this shader
		pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer
}
