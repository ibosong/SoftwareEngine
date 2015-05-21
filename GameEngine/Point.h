#pragma once
#include "Color.h"
#include "directxmath.h"
namespace GameEngine
{
	struct Point
	{
		// x, y are in the screen space, z represents the point's depth value
		DirectX::XMFLOAT3 screenPos;

		float lightIntensity;

		DirectX::XMFLOAT2 texCoord;

		Color color;
	};

}

