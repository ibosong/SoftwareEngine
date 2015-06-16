#pragma once
#include "directxmath.h"
#include "Texture.h"
#include "Light.h"
#include "Mesh.h"
#include "Point.h"
#include "LoadCMOModel.h"
#include <mutex>
namespace GameEngine
{
	class Rasterizer
	{
	public:
		Rasterizer();
		void SetFrameBuffer(byte* buffer, unsigned int width, unsigned int height);

		void Clear();

		void DrawLine(const Point &p1, const Point &p2, Texture* texture);
		void Paint(const Color & color);
		void DrawTriangle(Point p1, Point p2, Point p3, Texture* texture);
		inline void SetLight(const Light& light){ m_light = std::make_shared<Light>(light); }

		void XM_CALLCONV RenderMeth(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix);

		void XM_CALLCONV RenderX(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix);

		void XM_CALLCONV RenderZ(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix);

		void XM_CALLCONV RenderY(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix);

		void XM_CALLCONV RenderCMOModel(CMOModel* model, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix);

	private:
		void SetPixel(unsigned int x, unsigned int y, const Color &color);
		void SetPixel(float x, float y, const Color &color);
		void SetPixel(unsigned int x, unsigned int y, double z, const Color &color);
		void SortTrianglePoints(Point &pt1, Point &pt2, Point &pt3);

		float ComputeLightIntensity(DirectX::XMFLOAT3 dotLight, DirectX::XMFLOAT3 vertex, DirectX::XMFLOAT3 normal);
		// Project vertex to screen space
		Point XM_CALLCONV Project(const Vertex& vertex, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix);

		byte* m_buffer;
		unsigned int m_pixelWidth;
		unsigned int m_pixelHeight;
		std::vector<double> zbuffer;
		std::shared_ptr<Light>		m_light;

		std::mutex* m_mutexBuffer;
	};

}

