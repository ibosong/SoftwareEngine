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

		void DrawLine(const Vertex &v1, const Vertex &v2);
		void Paint(const Color & color);
		void DrawTriangle(Vertex v1, Vertex v2, Vertex v3);

		void XM_CALLCONV RenderMeth(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix);

		void XM_CALLCONV RenderX(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix);

		void XM_CALLCONV RenderZ(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix);

		void XM_CALLCONV RenderY(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix);

	private:
		float Interpolate(float min, float max, float percentage);
		void SortTrianglePoints(Vertex &pt1, Vertex &pt2, Vertex &pt3);
		// Project vertex to screen space
		void XM_CALLCONV Project(DirectX::XMFLOAT3& coord, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix);
		byte* m_buffer;
		unsigned int m_pixelWidth;
		unsigned int m_pixelHeight;
		std::vector<double> zbuffer;
	};

}

