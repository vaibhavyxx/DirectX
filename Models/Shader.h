#pragma once
#include <wrl/client.h>
#include <d3d11.h>
class Shader
{
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	ID3DBlob* vertexShaderBlob;
	ID3DBlob* pixelShaderBlob;

public:
	void Setup();
	void LoadShaders();

	void LoadPixelShader();
	void LoadVertexShader();
	void SetInputLayout();
};

