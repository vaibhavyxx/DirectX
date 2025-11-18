#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <iostream>
#include "Mesh.h"
#include "Camera.h"

class Sky {
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthBuffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> raster;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	std::shared_ptr<Mesh> geometry;

public:
	Sky(std::shared_ptr<Mesh> mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState);
	void Draw(float deltaTime, std::shared_ptr<Camera> cam);
};
