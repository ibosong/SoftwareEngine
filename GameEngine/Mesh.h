#pragma once
#include "directxmath.h"
#include "Color.h"
#include "Texture.h"
namespace GameEngine
{
	struct Vertex
	{
		Vertex(DirectX::XMFLOAT3 pos, GameEngine::Color color, DirectX::XMFLOAT2 textureCoor) :
			position(pos), color(color), texCoor(textureCoor), depth(pos.z)
		{

		}
		DirectX::XMFLOAT3	position;
		GameEngine::Color	color;
		DirectX::XMFLOAT2	texCoor;
		float				depth;
	};

	class Mesh
	{
	public:
		Mesh();
		Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned short>& indices, const Texture& texture);
		std::vector<Vertex>			Vertices;
		std::vector<unsigned short> Indices;
		Texture						texture;
	};
}

