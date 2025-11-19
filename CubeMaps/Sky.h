#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <iostream>
#include "Mesh.h"
#include "Camera.h"
#include <vector>
#include "Shader.h"

class Sky {
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthBuffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> raster;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	std::vector< Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> srvVectors;
	std::shared_ptr<Mesh> geometry;
	std::shared_ptr<Shader> shader;

public:
	Sky(std::shared_ptr<Mesh> mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState,
		Microsoft::WRL::ComPtr<ID3D11Texture2D> textures[6], std::shared_ptr<Shader> skyShader);
	void Draw(float deltaTime, std::shared_ptr<Camera> cam);
	// Helper for creating a cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		Microsoft::WRL::ComPtr<ID3D11Texture2D> textures[6]);
		/*const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);*/
};
