#include "pch.h"
#include "Rasterizer.h"
#include "algorithm"
#include "ppl.h"
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

	m_mutexBuffer = new std::mutex[width * height];
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
	
	std::lock_guard<std::mutex> locker(m_mutexBuffer[y * m_pixelHeight + x]);

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

float Rasterizer::ComputeLightIntensity(XMFLOAT3 dotLight, XMFLOAT3 vertex, XMFLOAT3 normal)
{
	// Compute cosin between light's direction and normal
	XMVECTOR lightVec = XMLoadFloat3(&dotLight);
	XMVECTOR vertexVec = XMLoadFloat3(&vertex);
	XMVECTOR normalVec = XMVector3Normalize(XMLoadFloat3(&normal));

	XMVECTOR lightDir = XMVector3Normalize(lightVec - vertexVec);

	return max(0.f, XMVector3Dot(normalVec, lightDir).m128_f32[0]);
}

Point XM_CALLCONV Rasterizer::Project(const Vertex& vertex, DirectX::FXMMATRIX worldMatrix,
	DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix)
{
	Point ret;
	XMVECTOR worldPosVec = XMVector3TransformCoord(XMLoadFloat3(&vertex.position), worldMatrix * viewMatrix * projectMatrix);

	XMFLOAT3 worldPos;
	XMStoreFloat3(&worldPos, worldPosVec);	


	XMVECTOR worldNormalVec;
	worldNormalVec = XMVector3TransformCoord(XMLoadFloat3(&vertex.normal), worldMatrix * viewMatrix * projectMatrix);

	XMFLOAT3 worldNormal;
	XMStoreFloat3(&worldNormal, worldNormalVec);

	if (m_light != nullptr)
	{
		ret.lightIntensity = ComputeLightIntensity(m_light->GetDotLight(), worldPos, worldNormal);
	}
	else
	{
		ret.lightIntensity = 1.f;
	}

	ret.screenPos.x = worldPos.x * m_pixelWidth + m_pixelWidth / 2.0f;
	ret.screenPos.y = -worldPos.y * m_pixelHeight + m_pixelHeight / 2.0f;
	ret.screenPos.z = worldPos.z;

	ret.texCoord = vertex.texCoord;

	ret.color = vertex.color;
	return ret;
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

void Rasterizer::DrawLine(const Point &p1, const Point &p2, Texture* texture)
{
	// Copy the parameters.
	int x1 = static_cast<int>(p1.screenPos.x);
	int y1 = static_cast<int>(p1.screenPos.y);
	XMFLOAT2 texCoor1 = p1.texCoord;
	float lightIntensity1 = p1.lightIntensity;
	double z1 = p1.screenPos.z;
	int x2 = static_cast<int>(p2.screenPos.x);
	int y2 = static_cast<int>(p2.screenPos.y);
	double z2 = p2.screenPos.z;
	XMFLOAT2 texCoor2 = p2.texCoord;
	float lightIntensity2 = p2.lightIntensity;

	Color color1 = p1.color;
	Color color2 = p2.color;

	Color color = color1;
	Color textureColor = Color();
	float dotLightIntensity = 1.f;
	
	if (x1 == x2 && y1 == y2)
	{
		if (texture != nullptr)
		{
			textureColor = texture->Map(p1.texCoord.x, p1.texCoord.y);
		}
		if (m_light != nullptr)
		{
			dotLightIntensity = lightIntensity1;
		}
		SetPixel(x1, y1, z1, color * textureColor * dotLightIntensity);
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
			std::swap(lightIntensity1, lightIntensity2);
		}

		Color colorStep = (color2 - color1) * (1.f / (y2 - y1));
		float lightStep = (lightIntensity2 - lightIntensity1) / (y2 - y2);
		XMFLOAT2 texCoorStep = XMFLOAT2((texCoor2.x - texCoor1.x) / (y2 - y1),
			(texCoor2.y - texCoor1.y) / (y2 - y1));
		color = color1;
		dotLightIntensity = lightIntensity1;
		XMFLOAT2 tempTexCoor = texCoor1;
		
		double zstep = (z2 - z1) * (1.f / (y2 - y1));
		double z = z1;
		for (int i = y1; i < y2; ++i)
		{
			if (texture != nullptr)
			{
				textureColor = texture->Map(tempTexCoor.x, tempTexCoor.y);
			}
			SetPixel(x1, i, z, color * textureColor * dotLightIntensity);
			color = color + colorStep;
			if (m_light != nullptr)
			{
				dotLightIntensity += lightStep;
			}
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
			std::swap(lightIntensity1, lightIntensity2);
		}

		Color colorStep = (color2 - color1) * (1.f / (x2 - x1));
		float lightStep = (lightIntensity2 - lightIntensity1) / (x2 - x1);
		XMFLOAT2 texCoorStep = XMFLOAT2((texCoor2.x - texCoor1.x) / (x2 - x1),
			(texCoor2.y - texCoor1.y) / (x2 - x1));
		color = color1;
		dotLightIntensity = lightIntensity1;
		XMFLOAT2 tempTexCoor = texCoor1;

		double zstep = (z2 - z1) * (1.f / (x2 - x1));
		double z = z1;
		for (int i = x1; i < x2; ++i)
		{
			if (texture != nullptr)
			{
				textureColor = texture->Map(tempTexCoor.x, tempTexCoor.y);
			}

			SetPixel(i, y1, z, color * textureColor * dotLightIntensity);
			color = color + colorStep;
			if (m_light != nullptr)
			{
				dotLightIntensity += lightStep;
			}
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
			std::swap(lightIntensity1, lightIntensity2);
		}

		xdiff = static_cast<float>(x2 - x1);
		ydiff = static_cast<float>(y2 - y1);
		float slope = std::abs(ydiff / xdiff);

		int ystep = ydiff > 0 ? 1 : -1;
		
		double zstep = (z2 - z1) * (1.f / (x2 - x1));
		float error = 0.f;

		int y = y1;


		Color colorStep = (color2 - color1) * (1.f / (x2 - x1));
		float lightStep = (lightIntensity2 - lightIntensity1) / (x2 - x1);
		XMFLOAT2 texCoorStep = XMFLOAT2((texCoor2.x - texCoor1.x) / (x2 - x1),
			(texCoor2.y - texCoor1.y) / (x2 - x1));
		color = color1;
		dotLightIntensity = lightIntensity1;
		XMFLOAT2 tempTexCoor = texCoor1;
		double z = z1;
		for (int x = x1; x < x2; ++x)
		{
			if (texture != nullptr)
			{
				textureColor = texture->Map(tempTexCoor.x, tempTexCoor.y);
			}

			z += zstep;
			error += slope;
			if (std::abs(error) > 0.5f)
			{
				y += ystep;
				error -= 1;
			}
			if (steep > 1)
			{
				SetPixel(y, x, z, color * textureColor * dotLightIntensity);
			}
			else
			{

				SetPixel(x, y, z, color * textureColor * dotLightIntensity);
			}

			color = color + colorStep;
			if (m_light != nullptr)
			{
				dotLightIntensity += lightStep;
			}
			tempTexCoor = XMFLOAT2(tempTexCoor.x + texCoorStep.x, tempTexCoor.y + texCoorStep.y);
		}
	}
	
}

void Rasterizer::DrawTriangle(Point p1, Point p2, Point p3, Texture* texture)
{
	// Make sure that p1.screenPos.y < p2.screenPos.y < p3.screenPos.y
	SortTrianglePoints(p1, p2, p3);

	int x1 = static_cast<int>(p1.screenPos.x);
	int y1 = static_cast<int>(p1.screenPos.y);
	float lightIntensity1 = p1.lightIntensity;
	int x2 = static_cast<int>(p2.screenPos.x);
	int y2 = static_cast<int>(p2.screenPos.y);
	float lightIntensiry2 = p2.lightIntensity;
	int x3 = static_cast<int>(p3.screenPos.x);
	int y3 = static_cast<int>(p3.screenPos.y);
	float lightIntensity3 = p3.lightIntensity;
	//int x4 = 0;
	int y4 = 0;

	Color colorStep13 = Color();
	Color colorStep12 = Color();
	float lightStep13 = 0.f;
	float lightStep12 = 0.f;
	XMFLOAT2 texCoordStep13 = XMFLOAT2();
	XMFLOAT2 texCoordStep12 = XMFLOAT2();

	float scanX1 = 0.f;
	float scanX2 = 0.f;
	double scanZ1 = 0.f;
	double scanZ2 = 0.f;

	float invslope12 = 0.f;
	float invslope13 = 0.f;
	double depthInvslope12 = 0.f;
	double depthInvslope13 = 0.f;


	// Divide the triangle into 2 parts
	if (y3 - y1 != 0 )
	{
		y4 = y2;
		//x4 = (y4 - y1) * (x3 - x1) / (y3 - y1) + x1;
		//double z4 = (y4 - y1) *	(p3.screenPos.z - p1.screenPos.z) / (y3 - y1) + p1.screenPos.z;

		colorStep13 = (p3.color - p1.color) * (1 / (y3 - y1));
		lightStep13 = (p3.lightIntensity - p1.lightIntensity) / (y3 - y1);
		texCoordStep13 = XMFLOAT2((p3.texCoord.x - p1.texCoord.x) / (y3 - y1),
			(p3.texCoord.y - p1.texCoord.y) / (y3 - y1));

		float xdiff13 = static_cast<float>(x3 - x1);
		float ydiff13 = static_cast<float>(y3 - y1);
		double depthDiff13 = p3.screenPos.z - p1.screenPos.z;
		invslope13 = xdiff13 / ydiff13;
		depthInvslope13 = depthDiff13 / ydiff13;
		// The top triangle
		//
		if (y2 - y1 != 0)
		{			
			colorStep12 = (p2.color - p1.color) * (1 / (y2 - y1));
			
			XMFLOAT2 texCoordStep12 = XMFLOAT2((p2.texCoord.x - p1.texCoord.x) / (y2 - y1),
				(p2.texCoord.y - p1.texCoord.y) / (y2 - y1));

			XMFLOAT2 texCoor4(texCoordStep13.x * (y4 - y1), texCoordStep13.y * (y4 - y1));

			float xdiff12 = static_cast<float>(x2 - x1);
			float ydiff12 = static_cast<float>(y2 - y1);
			double depthDiff12 = p2.screenPos.z - p1.screenPos.z;
			invslope12 = xdiff12 / ydiff12;
			depthInvslope12 = depthDiff12 / ydiff12;

			scanX1 = static_cast<float>(x1);
			scanX2 = static_cast<float>(x1);

			scanZ1 = p1.screenPos.z;
			scanZ2 = p1.screenPos.z;

			Color color14 = p1.color;
			Color color12 = p1.color;
			float light14 = p1.lightIntensity;
			float light12 = p1.lightIntensity;
			XMFLOAT2 texCoord14 = p1.texCoord;
			XMFLOAT2 texCoord12 = p1.texCoord;
			
			for (int i = y1; i < y2; ++i)
			{
				Point lineStart = { XMFLOAT3(scanX1, (float)i, (float)scanZ1), light12, texCoord12, color12 };
				Point lineEnd = { XMFLOAT3(scanX2, (float)i, (float)scanZ2), light14, texCoord14, color14 };
				DrawLine(lineStart, lineEnd, texture);

				scanX1 += invslope12;
				scanX2 += invslope13;
				scanZ1 += depthInvslope12;
				scanZ2 += depthInvslope13;
				color12 = color12 + colorStep12;
				color14 = color14 + colorStep13;
				light12 += lightStep12;
				light14 += lightStep13;
				texCoord12 = XMFLOAT2(texCoord12.x + texCoordStep12.x, texCoord12.y + texCoordStep12.y);
				texCoord14 = XMFLOAT2(texCoord14.x + texCoordStep13.x, texCoord14.y + texCoordStep13.y);
			}

		}
		
		// Draw the lower triangle
		if (y3 - y2 != 0)
		{
			float xdiff23 = x3 - x2;
			float ydiff23 = y3 - y2;

			Color colorStep23 = (p3.color - p2.color) * (1 / ydiff23);
			float lightStep23 = (p3.lightIntensity - p2.lightIntensity) / ydiff23;
			XMFLOAT2 texCoordStep23 = XMFLOAT2((p3.texCoord.x - p2.texCoord.x) / ydiff23,
				(p3.texCoord.y - p2.texCoord.y) / ydiff23);


			double depthDiff23 = p3.screenPos.z - p2.screenPos.z;

			float invslop23 = xdiff23 / ydiff23;
			double depthInvslope23 = depthDiff23 / ydiff23;

			scanX1 = x3;
			scanX2 = x3;
			scanZ1 = p3.screenPos.z;
			scanZ2 = p3.screenPos.z;

			Color color34 = p3.color;
			Color color32 = p3.color;
			float light34 = p3.lightIntensity;
			float light32 = p3.lightIntensity;
			XMFLOAT2 texCoor34 = p3.texCoord;
			XMFLOAT2 texCoor32 = p3.texCoord;

			for (int i = y3; i > y2 - 1; --i)
			{
				Point lineStart = { XMFLOAT3(scanX1, (float)i, (float)scanZ1), light32, texCoor32, color32 };
				Point lineEnd = { XMFLOAT3(scanX2, (float)i, (float)scanZ2), light34, texCoor34, color34 };
				DrawLine(lineStart, lineEnd, texture);

				scanX1 -= invslop23;
				scanX2 -= invslope13;
				scanZ1 -= depthInvslope23;
				scanZ2 -= depthInvslope13;
				color34 = color34 - colorStep13;
				color32 = color32 - colorStep23;
				light32 -= lightStep23;
				light34 -= lightStep13;
				texCoor32 = XMFLOAT2(texCoor32.x - texCoordStep23.x, texCoor32.y - texCoordStep23.y);
				texCoor34 = XMFLOAT2(texCoor34.x - texCoordStep13.x, texCoor34.y - texCoordStep13.y);
			}
		}
	}
}

void Rasterizer::SortTrianglePoints(Point &pt1, Point &pt2, Point &pt3)
{
	Point points[] = { pt1, pt2, pt3 };
	std::sort(points, points + 3, 
		[](Point p1, Point p2){
		return p1.screenPos.y < p2.screenPos.y;
	});
	pt1 = points[0];
	pt2 = points[1];
	pt3 = points[2];
}

void XM_CALLCONV Rasterizer::RenderMeth(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix)
{
	concurrency::parallel_for(size_t(0), mesh.Indices.size(), size_t(3), [=](size_t i){
		Vertex v1 = mesh.Vertices[mesh.Indices[i]];
		Vertex v2 = mesh.Vertices[mesh.Indices[i + 1]];
		Vertex v3 = mesh.Vertices[mesh.Indices[i + 2]];

		Point p1 = Project(v1, worldMatrix, viewMatrix, projectMatrix);
		Point p2 = Project(v2, worldMatrix, viewMatrix, projectMatrix);
		Point p3 = Project(v3, worldMatrix, viewMatrix, projectMatrix);

		DrawTriangle(p1, p2, p3, mesh.texture.get());
	});
	/*for (size_t i = 0; i < mesh.Indices.size(); i += 3)
	{
		Vertex v1 = mesh.Vertices[mesh.Indices[i]];
		Vertex v2 = mesh.Vertices[mesh.Indices[i + 1]];
		Vertex v3 = mesh.Vertices[mesh.Indices[i + 2]];

		Point p1 = Project(v1, worldMatrix, viewMatrix, projectMatrix);
		Point p2 = Project(v2, worldMatrix, viewMatrix, projectMatrix);
		Point p3 = Project(v3, worldMatrix, viewMatrix, projectMatrix);

		DrawTriangle(p1, p2, p3, mesh.texture.get());
	}*/
}

void XM_CALLCONV Rasterizer::RenderX(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix)
{
	for (int i = 24; i < 30; i += 3)
	{
		Vertex v1 = mesh.Vertices[mesh.Indices[i]];
		Vertex v2 = mesh.Vertices[mesh.Indices[i + 1]];
		Vertex v3 = mesh.Vertices[mesh.Indices[i + 2]];

		Point p1 = Project(v1, worldMatrix, viewMatrix, projectMatrix);
		Point p2 = Project(v2, worldMatrix, viewMatrix, projectMatrix);
		Point p3 = Project(v3, worldMatrix, viewMatrix, projectMatrix);

		DrawTriangle(p1, p2, p3, mesh.texture.get());
	}
}

void XM_CALLCONV Rasterizer::RenderZ(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix)
{
	for (int i = 0; i < 6; i += 3)
	{
		Vertex v1 = mesh.Vertices[mesh.Indices[i]];
		Vertex v2 = mesh.Vertices[mesh.Indices[i + 1]];
		Vertex v3 = mesh.Vertices[mesh.Indices[i + 2]];

		Point p1 = Project(v1, worldMatrix, viewMatrix, projectMatrix);
		Point p2 = Project(v2, worldMatrix, viewMatrix, projectMatrix);
		Point p3 = Project(v3, worldMatrix, viewMatrix, projectMatrix);

		DrawTriangle(p1, p2, p3, mesh.texture.get());
	}
}

void XM_CALLCONV Rasterizer::RenderY(const Mesh& mesh, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix)
{
	for (int i = 12; i < 18; i += 3)
	{
		Vertex v1 = mesh.Vertices[mesh.Indices[i]];
		Vertex v2 = mesh.Vertices[mesh.Indices[i + 1]];
		Vertex v3 = mesh.Vertices[mesh.Indices[i + 2]];
		Point p1 = Project(v1, worldMatrix, viewMatrix, projectMatrix);
		Point p2 = Project(v2, worldMatrix, viewMatrix, projectMatrix);
		Point p3 = Project(v3, worldMatrix, viewMatrix, projectMatrix);

		DrawTriangle(p1, p2, p3, mesh.texture.get());
	}
}

void XM_CALLCONV Rasterizer::RenderCMOModel(CMOModel* model, DirectX::FXMMATRIX worldMatrix, DirectX::FXMMATRIX viewMatrix, DirectX::FXMMATRIX projectMatrix)
{
	for (auto mesh : model->meshes)
	{
		RenderMeth(*mesh, worldMatrix, viewMatrix, projectMatrix);
	}
}