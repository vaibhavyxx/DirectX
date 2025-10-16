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
	//ID3DBlob* pixelShaderBlob;
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
	//ID3DBlob* vertexShaderBlob;
	D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);
	Graphics::Device->CreateVertexShader(
		vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize(),
		0,
		vertexShader.GetAddressOf()
	);
}

void Material::SetInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout)
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
		inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer*/
}


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
