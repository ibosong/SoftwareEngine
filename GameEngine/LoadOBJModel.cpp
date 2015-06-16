#include "pch.h"
#include "LoadOBJModel.h"
#include "memoryfile.h"
using namespace GameEngine;

OBJModel::OBJModel()
{
}


OBJModel::~OBJModel()
{
}

//std::unique_ptr<OBJModel> OBJModel::CreateFromOBJ(const char* fileName)
//{
//	memoryfile memfile;
//	memfile.Open(fileName);
//	if (memfile.IsOpen())
//	{
//		char* data = memfile.Data();
//
//	}
//}