#pragma once
#include <DirectXMath.h>

struct  ConstantBufferData
{
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT3 offset;
	float padding;
};