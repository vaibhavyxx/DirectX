#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <string>

class Shader
{
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	ID3DBlob* vertexShaderBlob;
	ID3DBlob* pixelShaderBlob;

public:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader();
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader();

	void Setup();
	void LoadShaders();

	void LoadPixelShader(std::string fileName);
	void LoadVertexShader();
	void SetInputLayout();

	void CopyBuffers(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, const void* srcData, size_t dataSize);
};

