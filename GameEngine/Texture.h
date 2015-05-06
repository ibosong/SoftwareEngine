#pragma once
#include "Color.h"
namespace GameEngine
{
	class Texture
	{
	public:
		Texture();
		Texture(int width, int height);
		Concurrency::task<void> Load(Platform::String^ filename);
		GameEngine::Color Map(float u, float v);
	private:
		byte* m_buffer;
		int m_width;
		int m_height;
	};
}


