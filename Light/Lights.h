#pragma once
#include <DirectXMath.h>
#include "Lights.h"

using namespace DirectX;

#define LIGHT_TYPE_DIRECTIONAL		0
#define LIGHT_TYPE_POINT			1
#define LIGHT_TYPE_SPOT				2

struct Light {
	int Type;
	XMFLOAT3 Direction;
	float Range;
	XMFLOAT3 Possition;
	float Intensity;
	XMFLOAT3 Color;
	float SpotInnerAngle;
	float SpotOuterAngle;
	XMFLOAT2 Padding;
};