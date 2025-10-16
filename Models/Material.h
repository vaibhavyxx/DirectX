#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>

using namespace DirectX;
class Material
{
private:
	XMFLOAT4 colorTint;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	ID3DBlob* vertexShaderBlob;
	ID3DBlob* pixelShaderBlob;
public:
	Material(Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader,
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader, DirectX::XMFLOAT4 color);

	//Setters 
	void SetColorTint(int r, int g, int b, int a);
	void SetColorTint(XMFLOAT4 color);

	void SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader);
	void SetPixelBuffer(Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader);

	void LoadPixelShader();
	void LoadVertexShader();

	void SetInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout);
	//Getters
	XMFLOAT4 GetColorTint;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader;
};

