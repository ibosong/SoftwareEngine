#pragma once
#include "Color.h"
namespace GameEngine
{
	class Texture
	{
	public:
		static Concurrency::task<std::shared_ptr<Texture>> Load(Platform::String^ filename);
		Texture(byte* buffer, int width, int height);		
		GameEngine::Color Map(float u, float v);
	private:
		static byte* static_buffer;
		byte* m_buffer;
		int m_width;
		int m_height;
	};
}


