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
	DirectX::XMFLOAT3 tanegent;
};

struct PixelStruct {
	Light lights[5];
	DirectX::XMFLOAT4 colorTint;      
	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 offset;    
	float time;
	DirectX::XMFLOAT3 camPos;
	float roughness;
	int type;
	int lightCount;
	bool useGamma;      
	bool useNormals;
	bool useRoughness;
	bool useMetals;
	bool useAldedo;
};

struct Skybox {
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};