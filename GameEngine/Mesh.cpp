#include "pch.h"
#include "Mesh.h"
using namespace GameEngine;

Mesh::Mesh()

{
}

void Mesh::Initialize(const std::vector<Vertex>& vertices, const std::vector<unsigned short> indices)
{
	Vertices = std::vector<Vertex>(vertices);
	Indices = std::vector<unsigned short>(indices);
}