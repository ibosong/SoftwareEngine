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

void XM_CALLCONV Rasterizer::Project(DirectX::XMFLOAT3& coord, DirectX::FXMMATRIX worldMatrix,
	DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix)
{
	XMVECTOR transVert = XMVector3TransformCoord(XMLoadFloat3(&coord), worldMatrix * viewMatrix * projectMatrix);

	XMFLOAT3 pt;
	XMStoreFloat3(&pt, transVert);

	coord.x = pt.x * m_pixelWidth + m_pixelWidth / 2.0f;
	coord.y = -pt.y * m_pixelHeight + m_pixelHeight / 2.0f;
	coord.z = pt.z;
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

void Rasterizer::DrawLine(const Vertex &v1, const Vertex &v2)
{
	// Copy the parameters.
	int x1 = static_cast<int>(v1.position.x);
	int y1 = static_cast<int>(v1.position.y);
	double z1 = v1.depth;
	int x2 = static_cast<int>(v2.position.x);
	int y2 = static_cast<int>(v2.position.y);
	double z2 = v2.depth;

	Color color1 = v1.color;
	Color color2 = v2.color;
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
		for (int i = y1; i < y2; ++i)
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
		for (int i = x1; i < x2; ++i)
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

		int ystep = ydiff > 0 ? 1 : -1;
		
		double zstep = (z2 - z1) * (1.f / (x2 - x1));
		float error = 0.f;

		int y = y1;


		Color colorStep = (color2 - color1) * (1.f / (x2 - x1));

		Color tempColor = color1;

		double z = z1;
		for (int x = x1; x < x2; ++x)
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

void Rasterizer::DrawTriangle(Vertex v1, Vertex v2, Vertex v3)
{
	int x1 = static_cast<int>(v1.position.x);
	int y1 = static_cast<int>(v1.position.y);
	int x2 = static_cast<int>(v2.position.x);
	int y2 = static_cast<int>(v2.position.y);
	int x3 = static_cast<int>(v3.position.x);
	int y3 = static_cast<int>(v3.position.y);

	// Make sure that v1.position.y < v2.position.y < v3.position.y
	SortTrianglePoints(v1, v2, v3);
	// Divide the triangle into 2 parts
	// The top triangle
	// 
	int x4 = 0, y4 = static_cast<int>(v2.position.y);
	x4 = (y4 - y1) * (x3 - x1) / (y3 - y1) + x1;
	double z4 = (y4 - y1) *	(v3.position.z - v1.position.z) / (y3 - y1) + v1.position.z;
	
	Color colorStep13 = (v3.color - v1.color) * (1 / (y3 - y1));
	Color colorStep12 = (v2.color - v1.color) * (1 / (y2 - y1));

	XMFLOAT2 texCoorStep13 = XMFLOAT2((v3.texCoor.x - v1.texCoor.x) / (y3 - y1), 
		(v3.texCoor.y - v1.texCoor.y)  / (y3 - y1));
	XMFLOAT2 texCoorStep12 = XMFLOAT2((v2.texCoor.x - v1.texCoor.x) / (y2 - y1), 
		(v2.texCoor.y - v1.texCoor.y) / (y2 - y1));

	XMFLOAT2 texCoor4(texCoorStep13.x * (y4 - y1), texCoorStep13.y * (y4 - y1));

	Vertex v4(XMFLOAT3(x4, y4, z4), colorStep13 * (y4 - y1), texCoor4);

	float xdiff12 = static_cast<float>(x2 - x1);
	float ydiff12 = static_cast<float>(y2 - y1);
	double zdiff12 = v2.position.z - v1.position.z;
	float xdiff13 = static_cast<float>(x3 - x1);
	float ydiff13 = static_cast<float>(y3 - y1);
	double zdiff13 = v3.position.z - v1.position.z;
	float invslope12 = xdiff12 / ydiff12;
	float invslope13 = xdiff13 / ydiff13;

	double zinvslope12 = zdiff12 / ydiff12;
	double zinvslope13 = zdiff13 / ydiff13;

	float scanX1 = static_cast<float>(x1);
	float scanX2 = static_cast<float>(x1);

	double scanZ1 = v1.position.z;
	double scanZ2 = v1.position.z;

	Color color14 = v1.color;
	Color color12 = v1.color;
	XMFLOAT2 texCoor14 = v1.texCoor;
	XMFLOAT2 texCoor12 = v1.texCoor;
	for (int i = y1; i < y4; ++i)
	{
		DrawLine(Vertex(XMFLOAT3(scanX1, (float)i, scanZ1), color12, texCoor12),
			Vertex(XMFLOAT3(scanX2, (float)i, scanZ2), color14, texCoor14));
		scanX1 += invslope12;
		scanX2 += invslope13;
		scanZ1 += zinvslope12;
		scanZ2 += zinvslope13;
		color12 = color12 + colorStep12;
		color14 = color14 + colorStep13;
		texCoor12 = XMFLOAT2(texCoor12.x + texCoorStep12.x, texCoor12.y + texCoorStep12.y);
		texCoor14 = XMFLOAT2(texCoor14.x + texCoorStep13.x, texCoor14.y + texCoorStep13.y);
	}

	// Draw the lower triangle
	if ((y4 - y3) > 1.f || (y4 - y3) < -1.f)
	{
		Color colorStep23 = (v3.color - v2.color) * (1 / (y3 - y2));
		XMFLOAT2 texCoorStep23 = XMFLOAT2((v3.texCoor.x - v2.texCoor.x) / (y3 - y2),
			(v3.texCoor.y - v2.texCoor.y) / (y3 - y2));

		float xdiff32 = x2 - x3;
		float ydiff32 = y2 - y3;
		double zdiff32 = v2.position.z - v3.position.z;
		float invslop32 = xdiff32 / ydiff32;
		double zinvslop32 = zdiff32 / ydiff32;

		scanX1 = x3;
		scanX2 = x3;

		double scanZ1 = v3.position.z;
		double scanZ2 = v3.position.z;

		Color color34 = v3.color;
		Color color32 = v3.color;

		XMFLOAT2 texCoor34 = v3.texCoor;
		XMFLOAT2 texCoor32 = v3.texCoor;
		for (int i = static_cast<int>(v3.position.y); i > y2 - 1; --i)
		{
			DrawLine(Vertex(XMFLOAT3(scanX1, (float)i, scanZ1), color32, texCoor32),
				Vertex(XMFLOAT3(scanX2, (float)i, scanZ2), color34, texCoor34));
			scanX1 -= invslop32;
			scanX2 -= invslope13;
			scanZ1 -= zinvslop32;
			scanZ2 -= zinvslope13;
			color34 = color34 - colorStep13;
			color32 = color32 - colorStep23;
			texCoor32 = XMFLOAT2(texCoor32.x - texCoorStep23.x, texCoor32.y - texCoorStep23.y);
			texCoor34 = XMFLOAT2(texCoor34.x - texCoorStep13.x, texCoor34.y - texCoorStep13.y);
		}
	}
}

void Rasterizer::SortTrianglePoints(Vertex &pt1, Vertex &pt2, Vertex &pt3)
{
	Vertex points[] = { pt1, pt2, pt3 };
	std::sort(points, points + 3, 
		[](Vertex p1, Vertex p2){
		return p1.position.y < p2.position.y;
	});
	pt1 = points[0];
	pt2 = points[1];
	pt3 = points[2];
}

void XM_CALLCONV Rasterizer::RenderMeth(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix)
{
	for (size_t i = 0; i < mesh.Indices.size(); i += 3)
	{
		Vertex v1 = mesh.Vertices[mesh.Indices[i]];
		Vertex v2 = mesh.Vertices[mesh.Indices[i + 1]];
		Vertex v3 = mesh.Vertices[mesh.Indices[i + 2]];

		Project(v1.position, worldMatrix, viewMatrix, projectMatrix);
		Project(v2.position, worldMatrix, viewMatrix, projectMatrix);
		Project(v3.position, worldMatrix, viewMatrix, projectMatrix);

		DrawTriangle(v1, v2, v3);
	}
}

void XM_CALLCONV Rasterizer::RenderX(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix)
{
	for (int i = 6; i < 12; i += 3)
	{
		Vertex v1 = mesh.Vertices[mesh.Indices[i]];
		Vertex v2 = mesh.Vertices[mesh.Indices[i + 1]];
		Vertex v3 = mesh.Vertices[mesh.Indices[i + 2]];

		Project(v1.position, worldMatrix, viewMatrix, projectMatrix);
		Project(v2.position, worldMatrix, viewMatrix, projectMatrix);
		Project(v3.position, worldMatrix, viewMatrix, projectMatrix);

		DrawTriangle(v1, v2, v3);
	}
}

void XM_CALLCONV Rasterizer::RenderZ(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix)
{
	for (int i = 30; i < 36; i += 3)
	{
		Vertex v1 = mesh.Vertices[mesh.Indices[i]];
		Vertex v2 = mesh.Vertices[mesh.Indices[i + 1]];
		Vertex v3 = mesh.Vertices[mesh.Indices[i + 2]];

		Project(v1.position, worldMatrix, viewMatrix, projectMatrix);
		Project(v2.position, worldMatrix, viewMatrix, projectMatrix);
		Project(v3.position, worldMatrix, viewMatrix, projectMatrix);

		DrawTriangle(v1, v2, v3);
	}
}

void XM_CALLCONV Rasterizer::RenderY(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix)
{
	for (int i = 18; i < 23; i += 3)
	{
		Vertex v1 = mesh.Vertices[mesh.Indices[i]];
		Vertex v2 = mesh.Vertices[mesh.Indices[i + 1]];
		Vertex v3 = mesh.Vertices[mesh.Indices[i + 2]];

		Project(v1.position, worldMatrix, viewMatrix, projectMatrix);
		Project(v2.position, worldMatrix, viewMatrix, projectMatrix);
		Project(v3.position, worldMatrix, viewMatrix, projectMatrix);

		DrawTriangle(v1, v2, v3);
	}
}