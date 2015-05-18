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

void Rasterizer::DrawLine(const Vertex &v1, const Vertex &v2, Texture texture)
{
	// Copy the parameters.
	int x1 = static_cast<int>(v1.position.x);
	int y1 = static_cast<int>(v1.position.y);
	XMFLOAT2 texCoor1 = v1.texCoor;
	double z1 = v1.depth;
	int x2 = static_cast<int>(v2.position.x);
	int y2 = static_cast<int>(v2.position.y);
	double z2 = v2.depth;
	XMFLOAT2 texCoor2 = v2.texCoor;

	Color color1 = v1.color;
	Color color2 = v2.color;
	
	if (x1 == x2 && y1 == y2)
	{
		SetPixel(x1, y1, z1, /*color1 * */texture.Map(v1.texCoor.x, v1.texCoor.y));
		return;
	}
	
	if (x1 == x2)
	{
		if (y1 > y2)
		{
			std::swap(y1, y2);
			std::swap(z1, z2);
			std::swap(color1, color2);
			std::swap(texCoor1, texCoor2);
		}

		Color colorStep = (color2 - color1) * (1.f / (y2 - y1));
		XMFLOAT2 texCoorStep = XMFLOAT2((texCoor2.x - texCoor1.x) / (y2 - y1),
			(texCoor2.y - texCoor1.y) / (y2 - y1));
		Color tempColor = color1;
		XMFLOAT2 tempTexCoor = texCoor1;
		
		double zstep = (z2 - z1) * (1.f / (y2 - y1));
		double z = z1;
		for (int i = y1; i < y2; ++i)
		{
			Color textureColor = texture.Map(tempTexCoor.x, tempTexCoor.y);
			SetPixel(x1, i, z,  textureColor);
			tempColor = tempColor + colorStep;
			tempTexCoor = XMFLOAT2(tempTexCoor.x + texCoorStep.x, tempTexCoor.y + texCoorStep.y);
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
			std::swap(texCoor1, texCoor2);
		}

		Color colorStep = (color2 - color1) * (1.f / (x2 - x1));
		XMFLOAT2 texCoorStep = XMFLOAT2((texCoor2.x - texCoor1.x) / (x2 - x1),
			(texCoor2.y - texCoor1.y) / (x2 - x1));
		Color tempColor = color1;
		XMFLOAT2 tempTexCoor = texCoor1;

		double zstep = (z2 - z1) * (1.f / (x2 - x1));
		double z = z1;
		for (int i = x1; i < x2; ++i)
		{
			Color textureColor = texture.Map(tempTexCoor.x, tempTexCoor.y);
			SetPixel(i, y1, z,  textureColor);
			tempColor = tempColor + colorStep;
			tempTexCoor = XMFLOAT2(tempTexCoor.x + texCoorStep.x, tempTexCoor.y + texCoorStep.y);
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
			std::swap(texCoor1, texCoor2);
		}

		xdiff = static_cast<float>(x2 - x1);
		ydiff = static_cast<float>(y2 - y1);
		float slope = std::abs(ydiff / xdiff);

		int ystep = ydiff > 0 ? 1 : -1;
		
		double zstep = (z2 - z1) * (1.f / (x2 - x1));
		float error = 0.f;

		int y = y1;


		Color colorStep = (color2 - color1) * (1.f / (x2 - x1));
		XMFLOAT2 texCoorStep = XMFLOAT2((texCoor2.x - texCoor1.x) / (x2 - x1),
			(texCoor2.y - texCoor1.y) / (x2 - x1));
		Color tempColor = color1;
		XMFLOAT2 tempTexCoor = texCoor1;
		double z = z1;
		for (int x = x1; x < x2; ++x)
		{
			Color textureColor = texture.Map(tempTexCoor.x, tempTexCoor.y);
			z += zstep;
			error += slope;
			if (std::abs(error) > 0.5f)
			{
				y += ystep;
				error -= 1;
			}
			if (steep > 1)
			{
				SetPixel(y, x, z,  textureColor);
			}
			else
			{
				SetPixel(x, y, z,  textureColor);
			}

			tempColor = tempColor + colorStep;
			tempTexCoor = XMFLOAT2(tempTexCoor.x + texCoorStep.x, tempTexCoor.y + texCoorStep.y);
		}
	}
	
}

void Rasterizer::DrawTriangle(Vertex v1, Vertex v2, Vertex v3, Texture texture)
{
	// Make sure that v1.position.y < v2.position.y < v3.position.y
	SortTrianglePoints(v1, v2, v3);

	int x1 = static_cast<int>(v1.position.x);
	int y1 = static_cast<int>(v1.position.y);
	int x2 = static_cast<int>(v2.position.x);
	int y2 = static_cast<int>(v2.position.y);
	int x3 = static_cast<int>(v3.position.x);
	int y3 = static_cast<int>(v3.position.y);
	int x4 = 0;
	int y4 = 0;
	Color colorStep13 = Color();
	Color colorStep12 = Color();
	float scanX1 = 0.f;
	float scanX2 = 0.f;
	double scanZ1 = 0.f;
	double scanZ2 = 0.f;
	float invslope12 = 0.f;
	float invslope13 = 0.f;
	double zinvslope12 = 0.f;
	double zinvslope13 = 0.f;
	XMFLOAT2 texCoorStep13 = XMFLOAT2();
	XMFLOAT2 texCoorStep12 = XMFLOAT2();
	// Divide the triangle into 2 parts
	if (y3 - y1 != 0 )
	{
		y4 = y2;
		x4 = (y4 - y1) * (x3 - x1) / (y3 - y1) + x1;
		double z4 = (y4 - y1) *	(v3.position.z - v1.position.z) / (y3 - y1) + v1.position.z;

		colorStep13 = (v3.color - v1.color) * (1 / (y3 - y1));
		texCoorStep13 = XMFLOAT2((v3.texCoor.x - v1.texCoor.x) / (y3 - y1),
			(v3.texCoor.y - v1.texCoor.y) / (y3 - y1));

		float xdiff13 = static_cast<float>(x3 - x1);
		float ydiff13 = static_cast<float>(y3 - y1);
		double zdiff13 = v3.position.z - v1.position.z;
		invslope13 = xdiff13 / ydiff13;
		zinvslope13 = zdiff13 / ydiff13;
		// The top triangle
		//
		if (y2 - y1 != 0)
		{			
			colorStep12 = (v2.color - v1.color) * (1 / (y2 - y1));
			
			XMFLOAT2 texCoorStep12 = XMFLOAT2((v2.texCoor.x - v1.texCoor.x) / (y2 - y1),
				(v2.texCoor.y - v1.texCoor.y) / (y2 - y1));

			XMFLOAT2 texCoor4(texCoorStep13.x * (y4 - y1), texCoorStep13.y * (y4 - y1));

			Vertex v4(XMFLOAT3(x4, y4, z4), colorStep13 * (y4 - y1), texCoor4);

			float xdiff12 = static_cast<float>(x2 - x1);
			float ydiff12 = static_cast<float>(y2 - y1);
			double zdiff12 = v2.position.z - v1.position.z;
			invslope12 = xdiff12 / ydiff12;
			zinvslope12 = zdiff12 / ydiff12;


			scanX1 = static_cast<float>(x1);
			scanX2 = static_cast<float>(x1);

			scanZ1 = v1.position.z;
			scanZ2 = v1.position.z;

			Color color14 = v1.color;
			Color color12 = v1.color;
			XMFLOAT2 texCoor14 = v1.texCoor;
			XMFLOAT2 texCoor12 = v1.texCoor;
			for (int i = y1; i < y2; ++i)
			{
				DrawLine(Vertex(XMFLOAT3(scanX1, (float)i, (float)scanZ1), color12, texCoor12),
					Vertex(XMFLOAT3(scanX2, (float)i, (float)scanZ2), color14, texCoor14), texture);
				scanX1 += invslope12;
				scanX2 += invslope13;
				scanZ1 += zinvslope12;
				scanZ2 += zinvslope13;
				color12 = color12 + colorStep12;
				color14 = color14 + colorStep13;
				texCoor12 = XMFLOAT2(texCoor12.x + texCoorStep12.x, texCoor12.y + texCoorStep12.y);
				texCoor14 = XMFLOAT2(texCoor14.x + texCoorStep13.x, texCoor14.y + texCoorStep13.y);
			}

		}
		
		// Draw the lower triangle
		if (y3 - y2 != 0)
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

			scanZ1 = v3.position.z;
			scanZ2 = v3.position.z;

			Color color34 = v3.color;
			Color color32 = v3.color;

			XMFLOAT2 texCoor34 = v3.texCoor;
			XMFLOAT2 texCoor32 = v3.texCoor;
			for (int i = y3; i > y2 - 1; --i)
			{
				DrawLine(Vertex(XMFLOAT3(scanX1, (float)i, (float)scanZ1), color32, texCoor32),
					Vertex(XMFLOAT3(scanX2, (float)i, (float)scanZ2), color34, texCoor34), texture);
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

		DrawTriangle(v1, v2, v3, mesh.texture);
	}
}

void XM_CALLCONV Rasterizer::RenderX(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix)
{
	for (int i = 24; i < 30; i += 3)
	{
		Vertex v1 = mesh.Vertices[mesh.Indices[i]];
		Vertex v2 = mesh.Vertices[mesh.Indices[i + 1]];
		Vertex v3 = mesh.Vertices[mesh.Indices[i + 2]];

		Project(v1.position, worldMatrix, viewMatrix, projectMatrix);
		Project(v2.position, worldMatrix, viewMatrix, projectMatrix);
		Project(v3.position, worldMatrix, viewMatrix, projectMatrix);

		DrawTriangle(v1, v2, v3, mesh.texture);
	}
}

void XM_CALLCONV Rasterizer::RenderZ(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix)
{
	for (int i = 0; i < 6; i += 3)
	{
		Vertex v1 = mesh.Vertices[mesh.Indices[i]];
		Vertex v2 = mesh.Vertices[mesh.Indices[i + 1]];
		Vertex v3 = mesh.Vertices[mesh.Indices[i + 2]];

		Project(v1.position, worldMatrix, viewMatrix, projectMatrix);
		Project(v2.position, worldMatrix, viewMatrix, projectMatrix);
		Project(v3.position, worldMatrix, viewMatrix, projectMatrix);

		DrawTriangle(v1, v2, v3, mesh.texture);
	}
}

void XM_CALLCONV Rasterizer::RenderY(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix)
{
	for (int i = 12; i < 18; i += 3)
	{
		Vertex v1 = mesh.Vertices[mesh.Indices[i]];
		Vertex v2 = mesh.Vertices[mesh.Indices[i + 1]];
		Vertex v3 = mesh.Vertices[mesh.Indices[i + 2]];

		Project(v1.position, worldMatrix, viewMatrix, projectMatrix);
		Project(v2.position, worldMatrix, viewMatrix, projectMatrix);
		Project(v3.position, worldMatrix, viewMatrix, projectMatrix);

		DrawTriangle(v1, v2, v3, mesh.texture);
	}
}