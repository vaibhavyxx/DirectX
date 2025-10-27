#pragma once
#include <DirectXMath.h>

struct  VertexStruct
{
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
};

struct PixelStruct {
	DirectX::XMFLOAT4 colorTint;
	float time;                  
	float pad1[3];               
	DirectX::XMFLOAT2 offset;    
	DirectX::XMFLOAT2 uvScale;   
	
};
