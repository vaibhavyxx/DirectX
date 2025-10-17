#include "Shader.h"
#include "Graphics.h"
#include <d3dcompiler.h>
#include "PathHelpers.h"
#include <memory>

Microsoft::WRL::ComPtr<ID3D11VertexShader> Shader::GetVertexShader()
{
	return vertexShader;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> Shader::GetPixelShader()
{
	return pixelShader;
}
void Shader::Setup() {
	Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Graphics::Context->IASetInputLayout(inputLayout.Get());
	Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
	Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);
}
void Shader::LoadShaders() {
	LoadVertexShader();
	LoadPixelShader();
}

void Shader::LoadPixelShader() {
	//ID3DBlob* pixelShaderBlob;
	D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
	
	Graphics::Device->CreatePixelShader(
		pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
		pixelShaderBlob->GetBufferSize(),		// How big is that data?
		0,										// No classes in this shader
		pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer
}
void Shader::LoadVertexShader() {
	//ID3DBlob* vertexShaderBlob;
	D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

	Graphics::Device->CreateVertexShader(
		vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
		vertexShaderBlob->GetBufferSize(),		// How big is that data?
		0,										// No classes in this shader
		vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
}
void Shader::SetInputLayout() {
	
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
