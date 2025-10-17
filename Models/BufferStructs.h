#pragma once
#include <DirectXMath.h>

struct  ConstantBufferData
{
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 normal;
	//DirectX::XMFLOAT4 colorTint;
};