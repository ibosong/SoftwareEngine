#pragma once
namespace GameEngine
{
	struct ByteColor
	{
		ByteColor(byte r, byte g, byte b, byte a) :
			R(r), G(g), B(b), A(a)
		{

		}
		byte R, G, B, A;
	};


	class Color
	{
	public:
		float R, G, B, A;

		Color(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

		uint32_t ToUInt32() const;
		ByteColor ToByte() const;
		Color operator + (const Color &c) const;
		Color operator - (const Color &c) const;
		Color operator * (float f) const;
	};

}
