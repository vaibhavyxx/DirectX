#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <string>
#include <wrl/client.h>
#include <d3d11shadertracing.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

namespace Graphics
{
	// --- GLOBAL VARS ---

	// Primary D3D11 API objects
	inline Microsoft::WRL::ComPtr<ID3D11Device> Device;
	inline Microsoft::WRL::ComPtr<ID3D11DeviceContext> Context;
	inline Microsoft::WRL::ComPtr<ID3D11DeviceContext1> Context1;
	inline Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;

	// Rendering buffers
	inline Microsoft::WRL::ComPtr<ID3D11RenderTargetView> BackBufferRTV;
	inline Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthBufferDSV;

	// Debug Layer
	inline Microsoft::WRL::ComPtr<ID3D11InfoQueue> InfoQueue;
	
	//Ring Buffer
	inline Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBufferHeap;
	inline unsigned int cbHeapSizeInBytes;		//Size
	inline unsigned int cbHeapOffsetInBytes;	//Position

	// --- FUNCTIONS ---

	// Getters
	bool VsyncState();
	std::wstring APIName();

	// General functions
	HRESULT Initialize(unsigned int windowWidth, unsigned int windowHeight, HWND windowHandle, bool vsyncIfPossible);
	void ShutDown();
	void ResizeBuffers(unsigned int width, unsigned int height);

	// Debug Layer
	void PrintDebugMessages();

	//Binding Buffers
	void FillAndBindNextCB(
		void* data,
		unsigned int dataSizeInBytes,
		D3D11_SHADER_TYPE shaderType,
		unsigned int registerSlot);
}