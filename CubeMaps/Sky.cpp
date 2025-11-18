#include "Sky.h"
#include "Graphics.h"
#include "BufferHelpers.h"

Sky::Sky(std::shared_ptr<Mesh> mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState)
{
	{
		D3D11_RASTERIZER_DESC rasterizer = {};
		rasterizer.FillMode = D3D11_FILL_SOLID;
		rasterizer.CullMode = D3D11_CULL_FRONT;
		Graphics::Device->CreateRasterizerState(&rasterizer, raster.GetAddressOf());
	}

	{
		D3D11_DEPTH_STENCIL_DESC depthStencilState = {};
		depthStencilState.DepthEnable = true;
		depthStencilState.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		Graphics::Device->CreateDepthStencilState(&depthStencilState, depthBuffer.GetAddressOf());
	}
	this->geometry = mesh;
	this->samplerState = samplerState;
}

void Sky::Draw(float deltaTime, std::shared_ptr<Camera> cam)
{
	Graphics::Context->RSSetState(raster.Get());
	Graphics::Context->OMSetDepthStencilState(depthBuffer.Get(), 0);
	
	Graphics::Context->VSSetShader(vertexShader.Get(),0,0);
	Graphics::Context->PSSetShader(pixelShader.Get(),0,0);
	
	Skybox skybox = {};
	skybox.view = cam->GetView();
	skybox.projection = cam->GetProjection();
	Graphics::FillAndBindNextCB(&skybox, sizeof(Skybox), D3D11_VERTEX_SHADER, 0);

	Graphics::Context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	Graphics::Context->PSSetShaderResources(0, 1, srv.GetAddressOf());

	geometry->Draw();

	Graphics::Context->RSSetState(0); // Null (or 0) puts back the defaults
	Graphics::Context->OMSetDepthStencilState(0, 0);
}
