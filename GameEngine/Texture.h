#pragma once
#include "Color.h"
namespace GameEngine
{
	class Texture
	{
	public:
		Texture(byte* buffer, int width, int height);		
		GameEngine::Color Map(float u, float v);
		static Concurrency::task<std::shared_ptr<Texture>> Load(Platform::String^ filename);
	private:
		byte* m_buffer;
		int m_width;
		int m_height;
	};
}


