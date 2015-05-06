#include "pch.h"
#include "Mesh.h"
using namespace GameEngine;

Mesh::Mesh()
{

}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned short> indices, Texture texture) :
Vertices(vertices), Indices(indices), texture(texture)
{

}
