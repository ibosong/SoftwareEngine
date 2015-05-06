#pragma once
#include "Color.h"
namespace GameEngine
{
	class Texture
	{
	public:
		Texture();
		Texture(Platform::String^ filename, int width, int height);
		void Load(Platform::String^ filename, int width, int height);
		GameEngine::Color Map(float u, float v);
	private:
		byte* m_buffer;
		int m_width;
		int m_height;
	};
}


