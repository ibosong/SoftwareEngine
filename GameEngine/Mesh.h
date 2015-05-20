#pragma once
#include "directxmath.h"
#include "Color.h"
#include "Texture.h"
namespace GameEngine
{
	struct Vertex
	{
		Vertex(DirectX::XMFLOAT3 pos,DirectX::XMFLOAT3 normal, GameEngine::Color color, DirectX::XMFLOAT2 textureCoor) :
			position(pos), normal(normal), color(color), texCoor(textureCoor), depth(pos.z)
		{

		}
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT3	normal;
		GameEngine::Color	color;
		DirectX::XMFLOAT2	texCoor;
		float				depth;
	};

	class Mesh
	{
	public:
		Mesh();
		Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned short>& indices, std::shared_ptr<Texture> texture);
		std::vector<Vertex>			Vertices;
		std::vector<unsigned short> Indices;
		std::shared_ptr<Texture>	texture;
	};
}

