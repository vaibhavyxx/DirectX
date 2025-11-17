#pragma once

#include <DirectXMath.h>

// --------------------------------------------------------
// A custom vertex definition
//
// You will eventually ADD TO this, and/or make more of these!
// --------------------------------------------------------
struct Vertex
{
	DirectX::XMFLOAT3 Position;	    // The local position of the vertex
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT3 Tangent;
};

/*
matrix world;
    matrix view;
    matrix projection;
    float3 normal;
    float pad;
    float3 worldPos;
    float pad2;
    matrix worldInvTranspose;
    float3 tangent;*/