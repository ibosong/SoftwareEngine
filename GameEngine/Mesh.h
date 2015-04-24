#pragma once
#include "directxmath.h"
#include "Color.h"
namespace GameEngine
{
	struct Vertex
	{
		Vertex(DirectX::XMFLOAT3 pos, GameEngine::Color color) :
			position(pos), color(color), depth(pos.z)
		{

		}
		DirectX::XMFLOAT3	position;
		GameEngine::Color				color;
		float				depth;
	};

	class Mesh
	{
	public:
		Mesh();
		void Initialize(const std::vector<Vertex>& vertices, const std::vector<unsigned short> indices);
		std::vector<Vertex> Vertices;
		std::vector<unsigned short> Indices;
	};
}

