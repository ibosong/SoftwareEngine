#pragma once
#include "Color.h"
namespace GameEngine
{
	class Point
	{
	public:
		Point(unsigned int x, unsigned int y, double depth, Color color);
		unsigned int x;
		unsigned int y;
		Color color;
		double depth;
	};

}

