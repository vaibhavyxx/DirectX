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
	Microsoft::WRL::ComPtr<ID3D11Buffer> pixelBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cb;

	ID3DBlob* vertexShaderBlob;
	ID3DBlob* pixelShaderBlob;

public:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader();
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetPixelBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetCB();

	void Setup();
	void LoadShaders();

	void LoadPixelShader(std::string fileName);
	void LoadVertexShader();
	void SetInputLayout();
	void CreatePixelBuffer();
	void CreateCB();

	void CopyBuffers(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, const void* srcData, size_t dataSize);
};

