#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include "Transform.h"
#include <memory>
#include "Camera.h"
#include "Shader.h"

using namespace DirectX;
class Material
{
private:
	XMFLOAT4 colorTint;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	//ID3DBlob* vertexShaderBlob;
	//ID3DBlob* pixelShaderBlob;
	std::shared_ptr<Shader> shader;
	float time;

public:
	Material(std::shared_ptr<Shader> shader, DirectX::XMFLOAT4 color);

	//Setters 
	void SetColorTint(int r, int g, int b, int a);
	void SetColorTint(XMFLOAT4 color);

	void SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader);
	void SetPixelBuffer(Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader);

	float GetTime();
	void SetTime(float value);

	//Getters
	XMFLOAT4 GetColorTint();
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader;
};

