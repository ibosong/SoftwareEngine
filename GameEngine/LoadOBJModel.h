#pragma once
#include "Mesh.h"
namespace GameEngine
{
	class OBJModel
	{
	public:
		std::shared_ptr<Mesh> mesh;
		OBJModel();
		~OBJModel();

		//static std::unique_ptr<OBJModel> CreateFromOBJ(const char* fileName);
	};

}

