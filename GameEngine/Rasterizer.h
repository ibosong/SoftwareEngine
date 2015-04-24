#pragma once
#include "Point.h"
#include "Mesh.h"
#include "directxmath.h"
namespace GameEngine
{
	class Rasterizer
	{
	public:
		Rasterizer();
		void SetFrameBuffer(byte* buffer, unsigned int width, unsigned int height);
		void SetPixel(unsigned int x, unsigned int y, const Color &color);
		void SetPixel(float x, float y, const Color &color);
		void SetPixel(unsigned int x, unsigned int y, double z, const Color &color);
		void Clear();

		void DrawLine(const Point &pt1, const Point &pt2);
		void Paint(const Color & color);
		void DrawTriangle(Point pt1, Point pt2, Point pt3);

		void RenderMeth(const Mesh& mesh, DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectMatrix);

		void RenderX(const Mesh& mesh, DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectMatrix);

		void RenderZ(const Mesh& mesh, DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectMatrix);

		void RenderY(const Mesh& mesh, DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectMatrix);

	private:
		float Interpolate(float min, float max, float percentage);
		void SortTrianglePoints(Point &pt1, Point &pt2, Point &pt3);
		// Project vertex to screen space
		DirectX::XMFLOAT3 Project(DirectX::XMFLOAT3 coord, DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectMatrix);
		byte* m_buffer;
		unsigned int m_pixelWidth;
		unsigned int m_pixelHeight;
		std::vector<double> zbuffer;
	};

}

