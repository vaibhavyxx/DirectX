#pragma once
#include <DirectXMath.h>
#include <iostream>

struct  ConstantBufferData
{
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	
};