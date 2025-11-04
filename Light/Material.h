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
	std::shared_ptr<Shader> shader;
	float time;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRVs[128];
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplers[16];

	int textureCounter;
	int samplerCounter;
	XMFLOAT2 uvOffset;
	XMFLOAT2 scale;
	float roughness;
	XMFLOAT3 ambient;

public:
	Material(std::shared_ptr<Shader> shader, DirectX::XMFLOAT4 color, float roughness, DirectX::XMFLOAT3 ambient);

	//Setters 
	void SetColorTint(int r, int g, int b, int a);
	void SetColorTint(XMFLOAT4 color);
	void SetRoughness(float value);
	void SetAmbient(float x, float y, float z);
	void SetAmbient(XMFLOAT3 value);
	XMFLOAT3 GetAmbient();
	void SetupPixelStruct(std::shared_ptr<Camera> cam, Light light);

	void SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader);
	void SetPixelBuffer(Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader);

	float GetTime();
	void SetTime(float value);

	XMFLOAT2 GetUVOffset();
	void SetUVOffset(XMFLOAT2 value);

	XMFLOAT2 GetScale();
	void SetScale(XMFLOAT2 value);
	void SetScale(float x, float y);

	void AddTextureSRV(unsigned int slot, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	void AddSampler(unsigned int slot, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);
	void BindTexturesAndSamplers();

	//Getters
	XMFLOAT4 GetColorTint();
	float GetRoughness();
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResourceView(int slot);
	std::shared_ptr<Shader> GetShader();

	int GetTextureCounter();
	int GetSamplerCounter();
};

