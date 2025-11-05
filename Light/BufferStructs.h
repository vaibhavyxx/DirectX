#pragma once
#include <DirectXMath.h>
#include "Lights.h"

struct  VertexStruct
{
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	DirectX::XMFLOAT3 normal;
	float pad;
	DirectX::XMFLOAT3 worldPos;
	float pad2;
	DirectX::XMFLOAT4X4 worldInvTranspose;
};

struct PixelStruct {
	DirectX::XMFLOAT4 colorTint;            
	DirectX::XMFLOAT2 offset;    
	DirectX::XMFLOAT2 uvScale;   
	float time;
	DirectX::XMFLOAT3 camPos;
	float roughness;
	DirectX::XMFLOAT3 ambient;
	Light light;
	int type;
};