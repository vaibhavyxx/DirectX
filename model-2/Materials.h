#pragma once
#include "Materials.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <iostream>
#include "PathHelpers.h"
#include "Graphics.h"

class Materials
{
private:
	DirectX::XMFLOAT4 colorTint;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

public:
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;
	Materials();
	DirectX::XMFLOAT4 GetColorTint();
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader();
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader();

	void LoadVertexShader();
	void LoadPixelShader();
};

