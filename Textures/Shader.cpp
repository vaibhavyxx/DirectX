#include "Shader.h"
#include "Graphics.h"
#include <d3dcompiler.h>
#include "PathHelpers.h"
#include <memory>
#include "BufferStructs.h"

Microsoft::WRL::ComPtr<ID3D11VertexShader> Shader::GetVertexShader()
{
	return vertexShader;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> Shader::GetPixelShader()
{
	return pixelShader;
}
Microsoft::WRL::ComPtr<ID3D11Buffer> Shader::GetPixelBuffer()
{
	return pixelBuffer;
}
Microsoft::WRL::ComPtr<ID3D11Buffer> Shader::GetCB()
{
	return cb;
}
void Shader::Setup() {
	Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Graphics::Context->IASetInputLayout(inputLayout.Get());
	Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
	Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);
}

void Shader::LoadPixelShader(std::string fileName) {
	std::wstring wideFileName(fileName.begin(), fileName.end());
	ID3DBlob* pixelShaderBlob;
	D3DReadFileToBlob(FixPath(wideFileName).c_str(), &pixelShaderBlob);

	Graphics::Device->CreatePixelShader(
		pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
		pixelShaderBlob->GetBufferSize(),		// How big is that data?
		0,										// No classes in this shader
		pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer
}
void Shader::LoadVertexShader() {
	ID3DBlob* vertexShaderBlob;
	D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

	Graphics::Device->CreateVertexShader(
		vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
		vertexShaderBlob->GetBufferSize(),		// How big is that data?
		0,										// No classes in this shader
		vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer

	SetInputLayout(vertexShaderBlob);
}
void Shader::SetInputLayout(ID3DBlob* vertexShaderBlob) {

	D3D11_INPUT_ELEMENT_DESC inputElements[3] = {};

	// Set up the first element - a position, which is 3 float values
	inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
	inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
	inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

	// Set up the second element - a uv, which is 2 more float values
	inputElements[1].Format = DXGI_FORMAT_R32G32_FLOAT;					// 2x 32-bit floats
	inputElements[1].SemanticName = "TEXCOORD";							// Match our vertex shader input!
	inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

	// Set up the third element - a normal, which is 3 more float values
	inputElements[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// 3x 32-bit floats
	inputElements[2].SemanticName = "NORMAL";							// Match our vertex shader input!
	inputElements[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

	// Create the input layout, verifying our description against actual shader code
	Graphics::Device->CreateInputLayout(
		inputElements,							// An array of descriptions
		3,										// How many elements in that array?
		vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
		vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
		inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer*/
}

void Shader::CreatePixelBuffer()
{
	{
		unsigned int size = sizeof(PixelStruct);
		size = (size + 15) / 16 * 16;

		D3D11_BUFFER_DESC cbDesc = {};
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.ByteWidth = size;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;

		Graphics::Device->CreateBuffer(&cbDesc, 0, pixelBuffer.GetAddressOf());
	}
}

void Shader::CreateCB()
{
	unsigned int size = sizeof(ConstantBufferData);
	size = (size + 15) / 16 * 16;

	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.ByteWidth = size;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;

	Graphics::Device->CreateBuffer(&cbDesc, 0, cb.GetAddressOf());
}

void Shader::FillAndBindCB(Microsoft::WRL::ComPtr<ID3D11Buffer> buffer, const void* data, int size, int slot, bool isVS)
{
	// Copy this data to the constant buffer we intend to use
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	Graphics::Context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

	// Straight memcpy() into the resource
	memcpy(mappedBuffer.pData, data, size);

	// Unmap so the GPU can once again use the buffer
	Graphics::Context->Unmap(buffer.Get(), 0);

	if(isVS)
		Graphics::Context->VSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
	else
		Graphics::Context->PSSetConstantBuffers(0, 1, buffer.GetAddressOf());;
}
