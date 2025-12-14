#pragma once
#include <DirectXMath.h>
#include "Lights.h"

struct  VertexStruct
{
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;
    DirectX::XMFLOAT4X4 worldInvTranspose;
    DirectX::XMFLOAT4X4 lightView;
    DirectX::XMFLOAT4X4 lightProjection;
};

struct PixelStruct {
    Light lights[5];
    DirectX::XMFLOAT4 colorTint;   //16
    DirectX::XMFLOAT2 scale;
    DirectX::XMFLOAT2 offset;      //32
    float time;
    DirectX::XMFLOAT3 camPos;      //48
    float roughness;
    int type;
    int lightCount;
    int useGamma;
    int useNormals;
    int useRoughness;
    int useMetals;
    int useSurfaceMap;     //96
    bool usePBR;
};

struct Skybox {
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

struct ShadowVSData {
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 proj;
};

struct BlurData {
    int blurRadius;
    float pixelWidth;
    float pixelHeight;
};

struct SolidColor {
    DirectX::XMFLOAT3 Color;
};

struct OutlineVSData {
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;
    float outlineSize;
};