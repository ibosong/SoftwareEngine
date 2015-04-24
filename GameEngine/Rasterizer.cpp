#include "pch.h"
#include "Rasterizer.h"
#include "algorithm"
using namespace DirectX;
using namespace GameEngine;

Rasterizer::Rasterizer()
{
}

void Rasterizer::SetFrameBuffer(byte* buffer, unsigned int width, unsigned int height)
{
	m_buffer = buffer;
	m_pixelWidth = width;
	m_pixelHeight = height;
	zbuffer = std::vector<double>(height * width, 1.0);
	
}

void Rasterizer::Clear()
{
	memset(m_buffer, 0, 4 * m_pixelHeight * m_pixelWidth);
	zbuffer.assign(zbuffer.size(), 1.0);
}

void Rasterizer::SetPixel(unsigned int x, unsigned int y, const Color &color)
{
	if (x >= m_pixelWidth || y >= m_pixelHeight)
		return;
	ByteColor byteColor = color.ToByte();
	
	m_buffer[(y * m_pixelWidth + x) * 4] = byteColor.B;
	m_buffer[(y * m_pixelWidth + x) * 4 + 1] = byteColor.G;
	m_buffer[(y * m_pixelWidth + x) * 4 + 2] = byteColor.R;
	m_buffer[(y * m_pixelWidth + x) * 4 + 3] = byteColor.A;
}

void Rasterizer::SetPixel(float x, float y, const Color &color)
{
	if (x < 0.0f || y < 0.0f)
		return;

	SetPixel((unsigned int)x, (unsigned int)y, color);
}

void Rasterizer::SetPixel(unsigned int x, unsigned int y, double z, const Color &color)
{
	if (zbuffer[y * m_pixelWidth + x] < z)
	{
		return;
	}

	zbuffer.at(y * m_pixelWidth + x) = z;
	SetPixel(x, y, color);
}

float Rasterizer::Interpolate(float min, float max, float percentage)
{
	// Clamp percentage to keep it between 0 and 1.
	percentage = max(0.f, min(1.f, percentage));

	return min + (max - min) * percentage;
}

DirectX::XMFLOAT3 Rasterizer::Project(DirectX::XMFLOAT3 coord, DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectMatrix)
{ 
	XMFLOAT4X4 transformMatrix;
	XMStoreFloat4x4(&transformMatrix, XMLoadFloat4x4(&viewMatrix) * XMLoadFloat4x4(&projectMatrix));
	XMVECTOR transVert = XMVector3TransformCoord(XMLoadFloat3(&coord), XMLoadFloat4x4(&transformMatrix));

	//XMVECTOR vertInViewSpace = XMVector3Transform(XMLoadFloat3(&coord), XMLoadFloat4x4(&viewMatrix));
	//XMFLOAT3 ptInViewSpace;
	//XMStoreFloat3(&ptInViewSpace, vertInViewSpace);
	XMFLOAT3 pt;
	XMStoreFloat3(&pt, transVert);

	auto x = pt.x * m_pixelWidth + m_pixelWidth / 2.0f;
	auto y = -pt.y * m_pixelHeight + m_pixelHeight / 2.0f;

	return XMFLOAT3(x, y, pt.z);
}


void Rasterizer::Paint(const Color &color)
{
	for (unsigned int y = 0; y < m_pixelHeight; y += 1) {
		for (unsigned int x = 0; x < m_pixelWidth; x += 1)
		{
			SetPixel(x, y, color);
		}
	}
}

void Rasterizer::DrawLine(const Point &pt1, const Point &pt2)
{
	// Copy the parameters.
	unsigned int x1 = pt1.x;
	unsigned int y1 = pt1.y;
	double z1 = pt1.depth;
	unsigned int x2 = pt2.x;
	unsigned int y2 = pt2.y;
	double z2 = pt2.depth;

	Color color1 = pt1.color;
	Color color2 = pt2.color;
	if (x1 == x2 && y1 == y2)
	{
		SetPixel(x1, y1, z1, color1);
		return;
	}
	
	if (x1 == x2)
	{
		if (y1 > y2)
		{
			std::swap(y1, y2);
			std::swap(z1, z2);
			std::swap(color1, color2);
		}

		Color colorStep = (color2 - color1) * (1.f / (y2 - y1));
		Color tempColor = color1;
		double zstep = (z2 - z1) * (1.f / (y2 - y1));
		double z = z1;
		for (unsigned int i = y1; i < y2; ++i)
		{
			SetPixel(x1, i, z, tempColor);
			tempColor = tempColor + colorStep;
			z += zstep;
		}
	}
	else if (y1 == y2)
	{
		if (x1 > x2)
		{
			std::swap(x1, x2);
			std::swap(z1, z2);
			std::swap(color1, color2);
		}

		Color colorStep = (color2 - color1) * (1.f / (x2 - x1));
		Color tempColor = color1;

		double zstep = (z2 - z1) * (1.f / (x2 - x1));
		double z = z1;
		for (unsigned int i = x1; i < x2; ++i)
		{
			SetPixel(i, y1, z, tempColor);
			tempColor = tempColor + colorStep;
			z += zstep;
		}
	}
	else
	{
		// Caculate the origin steep(or called slope)
		float xdiff = static_cast<float>(x2 - x1);
		float ydiff = static_cast<float>(y2 - y1);
		

		float steep = std::abs(ydiff / xdiff);
		

		if (steep > 1)
		{
			// Swap x and y.
			std::swap(x1, y1);
			std::swap(x2, y2);
		}

		if (x2 < x1)
		{
			// If x2 < x1, swap the direction of the line.
			std::swap(x1, x2);

			std::swap(y1, y2);

			std::swap(z1, z2);

			std::swap(color1, color2);
		}

		xdiff = static_cast<float>(x2 - x1);
		ydiff = static_cast<float>(y2 - y1);
		float slope = std::abs(ydiff / xdiff);

		unsigned int ystep = ydiff > 0 ? 1 : -1;
		
		double zstep = (z2 - z1) * (1.f / (x2 - x1));
		float error = 0.f;

		unsigned int y = y1;


		Color colorStep = (color2 - color1) * (1.f / (x2 - x1));

		Color tempColor = color1;

		double z = z1;
		for (unsigned int x = x1; x < x2; ++x)
		{
			z += zstep;
			error += slope;
			if (std::abs(error) > 0.5f)
			{
				y += ystep;
				error -= 1;
			}
			if (steep > 1)
			{
				SetPixel(y, x, z, tempColor);
			}
			else
			{
				SetPixel(x, y, z, tempColor);
			}

			tempColor = tempColor + colorStep;
		}
	}
	
}

void Rasterizer::DrawTriangle(Point pt1, Point pt2, Point pt3)
{
	// Make sure that pt1.y < pt2.y < pt3.y
	SortTrianglePoints(pt1, pt2, pt3);

	unsigned int x4 = 0, y4 = pt2.y;
	x4 = (y4 - pt1.y) * (pt3.x - pt1.x) / (pt3.y - pt1.y) + pt1.x;
	double z4 =	(y4 - pt1.y) *	(pt3.depth - pt1.depth) / (pt3.y - pt1.y) +	pt1.depth;
	
	Color colorStep13 = (pt3.color - pt1.color) * (1 / static_cast<float>(pt3.y - pt1.y));
	Color colorStep12 = (pt2.color - pt1.color) * (1 / static_cast<float>(pt2.y - pt1.y));
	Point pt4(x4, y4, z4, colorStep13 * static_cast<float>(y4 - pt1.y));

	float xdiff12 = static_cast<float>(static_cast<int>(pt2.x - pt1.x));
	float ydiff12 = static_cast<float>(static_cast<int>(pt2.y - pt1.y));
	double zdiff12 = pt2.depth - pt1.depth;
	float xdiff13 = static_cast<float>(static_cast<int>(pt3.x - pt1.x));
	float ydiff13 = static_cast<float>(static_cast<int>(pt3.y - pt1.y));
	double zdiff13 = pt3.depth - pt1.depth;
	float invslope12 = xdiff12 / ydiff12;
	float invslope13 = xdiff13 / ydiff13;

	double zinvslope12 = zdiff12 / ydiff12;
	double zinvslope13 = zdiff13 / ydiff13;

	float scanX1 = static_cast<float>(pt1.x);
	float scanX2 = static_cast<float>(pt1.x);

	double scanZ1 = pt1.depth;
	double scanZ2 = pt1.depth;

	Color color14 = pt1.color;
	Color color12 = pt1.color;
	for (unsigned int i = pt1.y; i < y4; ++i)
	{
		DrawLine(Point(static_cast<unsigned int>(scanX1), i, scanZ1, color12),
			Point(static_cast<unsigned int>(scanX2), i, scanZ2, color14));
		scanX1 += invslope12;
		scanX2 += invslope13;
		scanZ1 += zinvslope12;
		scanZ2 += zinvslope13;
		color12 = color12 + colorStep12;
		color14 = color14 + colorStep13;
	}

	// Draw the lower triangle
	if ((y4 - pt3.y) > 1.f || (y4 - pt3.y) < -1.f)
	{
		Color colorStep23 = (pt3.color - pt2.color) * (1 / static_cast<float>(pt3.y - pt2.y));

		float xdiff32 = static_cast<float>(static_cast<int>(pt2.x - pt3.x));
		float ydiff32 = static_cast<float>(static_cast<int>(pt2.y - pt3.y));
		double zdiff32 = pt2.depth - pt3.depth;
		float invslop32 = xdiff32 / ydiff32;
		double zinvslop32 = zdiff32 / ydiff32;

		scanX1 = static_cast<float>(pt3.x);
		scanX2 = static_cast<float>(pt3.x);

		double scanZ1 = pt3.depth;
		double scanZ2 = pt3.depth;

		Color color34 = pt3.color;
		Color color32 = pt3.color;
		for (unsigned int i = pt3.y; i > pt2.y - 1; --i)
		{
			DrawLine(Point(static_cast<unsigned int>(scanX1), i, scanZ1, color32),
				Point(static_cast<unsigned int>(scanX2), i, scanZ2, color34));
			scanX1 -= invslop32;
			scanX2 -= invslope13;
			scanZ1 -= zinvslop32;
			scanZ2 -= zinvslope13;
			color34 = color34 - colorStep13;
			color32 = color32 - colorStep23;
		}
	}
}

void Rasterizer::SortTrianglePoints(Point &pt1, Point &pt2, Point &pt3)
{
	Point points[] = { pt1, pt2, pt3 };
	std::sort(points, points + 3, 
		[](Point p1, Point p2){
		return p1.y < p2.y;
	});
	pt1 = points[0];
	pt2 = points[1];
	pt3 = points[2];
}

void Rasterizer::RenderMeth(const Mesh& mesh, DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectMatrix)
{
	for (size_t i = 0; i < mesh.Indices.size(); i += 3)
	{
		Vertex v1 = mesh.Vertices[mesh.Indices[i]];
		Vertex v2 = mesh.Vertices[mesh.Indices[i + 1]];
		Vertex v3 = mesh.Vertices[mesh.Indices[i + 2]];

		XMFLOAT3 pt1 = Project(v1.position, viewMatrix, projectMatrix);
		XMFLOAT3 pt2 = Project(v2.position, viewMatrix, projectMatrix);
		XMFLOAT3 pt3 = Project(v3.position, viewMatrix, projectMatrix);

		DrawTriangle(Point(static_cast<unsigned int>(pt1.x), static_cast<unsigned int>(pt1.y), pt1.z, v1.color),
			Point(static_cast<unsigned int>(pt2.x), static_cast<unsigned int>(pt2.y), pt2.z, v2.color),
			Point(static_cast<unsigned int>(pt3.x), static_cast<unsigned int>(pt3.y), pt3.z, v3.color));
	}
}

void Rasterizer::RenderX(const Mesh& mesh, DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectMatrix)
{
	for (int i = 6; i < 12; i += 3)
	{
		Vertex v1 = mesh.Vertices[mesh.Indices[i]];
		Vertex v2 = mesh.Vertices[mesh.Indices[i + 1]];
		Vertex v3 = mesh.Vertices[mesh.Indices[i + 2]];

		XMFLOAT3 pt1 = Project(v1.position, viewMatrix, projectMatrix);
		XMFLOAT3 pt2 = Project(v2.position, viewMatrix, projectMatrix);
		XMFLOAT3 pt3 = Project(v3.position, viewMatrix, projectMatrix);

		DrawTriangle(Point(pt1.x, pt1.y, pt1.z, v1.color),
			Point(pt2.x, pt2.y, pt2.z, v2.color),
			Point(pt3.x, pt3.y, pt3.z, v3.color));
	}
}

void Rasterizer::RenderZ(const Mesh& mesh, DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectMatrix)
{
	for (int i = 30; i < 36; i += 3)
	{
		Vertex v1 = mesh.Vertices[mesh.Indices[i]];
		Vertex v2 = mesh.Vertices[mesh.Indices[i + 1]];
		Vertex v3 = mesh.Vertices[mesh.Indices[i + 2]];

		XMFLOAT3 pt1 = Project(v1.position, viewMatrix, projectMatrix);
		XMFLOAT3 pt2 = Project(v2.position, viewMatrix, projectMatrix);
		XMFLOAT3 pt3 = Project(v3.position, viewMatrix, projectMatrix);

		DrawTriangle(Point(pt1.x, pt1.y, pt1.z, v1.color),
			Point(pt2.x, pt2.y, pt2.z, v2.color),
			Point(pt3.x, pt3.y, pt3.z, v3.color));
	}
}

void Rasterizer::RenderY(const Mesh& mesh, DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectMatrix)
{
	for (int i = 18; i < 23; i += 3)
	{
		Vertex v1 = mesh.Vertices[mesh.Indices[i]];
		Vertex v2 = mesh.Vertices[mesh.Indices[i + 1]];
		Vertex v3 = mesh.Vertices[mesh.Indices[i + 2]];

		XMFLOAT3 pt1 = Project(v1.position, viewMatrix, projectMatrix);
		XMFLOAT3 pt2 = Project(v2.position, viewMatrix, projectMatrix);
		XMFLOAT3 pt3 = Project(v3.position, viewMatrix, projectMatrix);

		DrawTriangle(Point(pt1.x, pt1.y, pt1.z, v1.color),
			Point(pt2.x, pt2.y, pt2.z, v2.color),
			Point(pt3.x, pt3.y, pt3.z, v3.color));
	}
}