#pragma once
#include <DirectXMath.h>

struct  ConstantBufferData
{
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT4X4 worldMatrix;
};