#pragma once
#include "directxmath.h"
#include "Color.h"
#include "Texture.h"
namespace GameEngine
{
	struct Vertex
	{
		// Data in 3D space
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT3	normal;
		GameEngine::Color	color;
		DirectX::XMFLOAT2	texCoord;
	};

	struct Mesh
	{
	public:
		std::vector<Vertex>			Vertices;
		std::vector<unsigned short> Indices;
		std::shared_ptr<Texture>	texture;
	};
}

