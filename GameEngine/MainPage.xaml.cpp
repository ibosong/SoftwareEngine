//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "Robuffer.h"
#include "Point.h"
using namespace GameEngine;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace DirectX;
// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

MainPage::MainPage() : m_pixelWidth(500), m_pixelHeight(500), m_texture()
{
	InitializeComponent();	
	
	//CompositionTarget::Rendering += ref new EventHandler<Object^>(this, &MainPage::OnRendering);
	
	

	std::vector<Vertex> vertices = {
			// +z face
			Vertex(XMFLOAT3(-0.3f, -0.3f, 0.3f), Color(1.f, 0.f, 0.f), XMFLOAT2(0.f, 0.f)),
			Vertex(XMFLOAT3(0.3f, -0.3f, 0.3f), Color(1.f, 0.f, 0.f),  XMFLOAT2(0.f, 1.f)),
			Vertex(XMFLOAT3(0.3f, 0.3f, 0.3f), Color(1.f, 0.f, 0.f),   XMFLOAT2(1.f, 0.f)),
			Vertex(XMFLOAT3(-0.3f, 0.3f, 0.3f), Color(1.f, 0.f, 0.f),  XMFLOAT2(1.f, 1.f)),
			// -z face
			Vertex(XMFLOAT3(-0.3f, -0.3f, -0.3f), Color(1.f, 1.f, 0.f), XMFLOAT2(0.f, 0.f)),
			Vertex(XMFLOAT3(-0.3f, 0.3f, -0.3f), Color(1.f, 1.f, 0.f),  XMFLOAT2(0.f, 1.f)),
			Vertex(XMFLOAT3(0.3f, 0.3f, -0.3f), Color(1.f, 1.f, 0.f),   XMFLOAT2(1.f, 0.f)),
			Vertex(XMFLOAT3(0.3f, -0.3f, -0.3f), Color(1.f, 1.f, 0.f),  XMFLOAT2(1.f, 1.f)),
			// +y face
			Vertex(XMFLOAT3(-0.3f, 0.3f, -0.3f), Color(0.f, 1.f, 0.f),  XMFLOAT2(0.f, 0.f)),
			Vertex(XMFLOAT3(-0.3f, 0.3f, 0.3f), Color(0.f, 1.f, 0.f),   XMFLOAT2(0.f, 1.f)),
			Vertex(XMFLOAT3(0.3f, 0.3f, 0.3f), Color(0.f, 1.f, 0.f),    XMFLOAT2(1.f, 0.f)),
			Vertex(XMFLOAT3(0.3f, 0.3f, -0.3f), Color(0.f, 1.f, 0.f),   XMFLOAT2(1.f, 1.f)),
			// -y face
			Vertex(XMFLOAT3(-0.3f, -0.3f, -0.3f), Color(1.f, 0.f, 1.f), XMFLOAT2(0.f, 0.f)),
			Vertex(XMFLOAT3(0.3f, -0.3f, -0.3f), Color(1.f, 0.f, 1.f),  XMFLOAT2(0.f, 1.f)),
			Vertex(XMFLOAT3(0.3f, -0.3f, 0.3f), Color(1.f, 0.f, 1.f),   XMFLOAT2(1.f, 0.f)),
			Vertex(XMFLOAT3(-0.3f, -0.3f, 0.3f), Color(1.f, 0.f, 1.f),  XMFLOAT2(1.f, 1.f)),
			// -x face
			Vertex(XMFLOAT3(-0.3f, -0.3f, 0.3f), Color(0.f, 0.f, 1.f),  XMFLOAT2(0.f, 0.f)),
			Vertex(XMFLOAT3(-0.3f, 0.3f,  0.3f), Color(0.f, 0.f, 1.f),  XMFLOAT2(0.f, 1.f)),
			Vertex(XMFLOAT3(-0.3f,  0.3f, -0.3f), Color(0.f, 0.f, 1.f), XMFLOAT2(1.f, 0.f)),
			Vertex(XMFLOAT3(-0.3f, -0.3f, -0.3f), Color(0.f, 0.f, 1.f), XMFLOAT2(1.f, 1.f)),
			// +x face
			Vertex(XMFLOAT3(0.3f, -0.3f, -0.3f), Color(0.f, 1.f, 1.f), XMFLOAT2(0.f, 0.f)),
			Vertex(XMFLOAT3(0.3f, 0.3f,  -0.3f), Color(0.f, 1.f, 1.f), XMFLOAT2(0.f, 1.f)),
			Vertex(XMFLOAT3(0.3f, 0.3f, 0.3f), Color(0.f, 1.f, 1.f),   XMFLOAT2(1.f, 0.f)),
			Vertex(XMFLOAT3(0.3f, -0.3f, 0.3f), Color(0.f, 1.f, 1.f),  XMFLOAT2(1.f, 1.f)),
			
			
	};

	std::vector<unsigned short> cubeIndices = {
		// Front Face
		0, 1, 2,
		0, 2, 3,

		// Back Face
		4, 5, 6,
		4, 6, 7,

		// Top Face
		8, 9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23
	};

	m_texture.Load("image.jpg").then([=](){
		m_cube = Mesh(vertices, cubeIndices, m_texture);
	});
	

	Size outputSize = Size(static_cast<float>(m_pixelWidth), static_cast<float>(m_pixelHeight));
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}
	m_projectMatrix = XMMatrixPerspectiveFovLH(
		fovAngleY,
		aspectRatio,
		1.0f,
		50.0f
		);

	// Eye is at (0,1.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	static const XMVECTORF32 eye = { 0.0f, 0.9f, 3.5f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	m_viewMatrix = XMMatrixLookAtLH(eye, at, up);


	//XMMATRIX transformMatrix = viewMatrix * perspectiveMatrix;

	

	m_bitmap = ref new WriteableBitmap(m_pixelWidth, m_pixelHeight);
	
	// Query the IBufferByteAccess interface.
	Microsoft::WRL::ComPtr<IBufferByteAccess> bufferByteAccess;
	reinterpret_cast<IInspectable*>(m_bitmap->PixelBuffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));

	// Retrieve the buffer data.
	m_buffer = nullptr;
	HRESULT hr = bufferByteAccess->Buffer(&m_buffer);
	if (FAILED(hr))
	{
		throw Exception::CreateException(hr);
	}
	m_rasterizer = Rasterizer();
	m_rasterizer.SetFrameBuffer(m_buffer, m_pixelWidth, m_pixelHeight);
	//rasterizer.DrawLine(Point(100, 10, Color(1, 0, 0)), Point(200, 10, Color(0, 0, 1)));
	//Paint(Color(1.f, 0.f, 0.f, 1.f));
	//


	imageRendering->Source = m_bitmap;
}


void MainPage::OnRendering(Platform::Object^ sender, Platform::Object^ args)
{
	m_rasterizer.Clear();
	static float angle = 0.f;
	angle += 0.05f;
	if (angle >= 3.14 * 2)
	{
		angle = 0.f;
	}
	//angle = std::fmod(angle, 3.14 * 2);
	XMMATRIX world = XMMatrixRotationY(angle);

	m_rasterizer.RenderMeth(m_cube, world, m_viewMatrix, m_projectMatrix);

	m_bitmap->Invalidate();
}

void MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	/*m_rasterizer.DrawTriangle(Point(300, 300, -1.f, Color(1, 0, 0)), 
		Point(100, 0, -1.f, Color(1, 0, 0)), Point(0, 100, -1.f, Color(1, 0, 0)));

	m_rasterizer.DrawTriangle(Point(200, 0, 2.f, Color(0, 1, 0)),
		Point(100, 0, 2.f, Color(0, 1, 0)), Point(0, 200, -4.f, Color(0, 1, 0)));


	m_bitmap->Invalidate();*/

	m_rasterizer.RenderMeth(m_cube, XMMatrixIdentity(), m_viewMatrix, m_projectMatrix);

	m_bitmap->Invalidate();
}


void MainPage::Button_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_rasterizer.Clear();
	
	m_bitmap->Invalidate();
}
// Handle pointer moved event to rotate the view space martrix.
void MainPage::OnPointerMoved(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args)
{
	m_rasterizer.Clear();
	static float angle = 0.f;
	angle += 0.01f;
	if (angle >= 3.14f * 2)
	{
		angle = 0.f;
	}

	m_rasterizer.RenderMeth(m_cube, XMMatrixRotationY(angle), m_viewMatrix, m_projectMatrix);
	m_bitmap->Invalidate();
}

void MainPage::Border_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_pointerMoveToken = Window::Current->CoreWindow->PointerMoved +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &MainPage::OnPointerMoved);
}


void MainPage::Border_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	Window::Current->CoreWindow->PointerMoved -= m_pointerMoveToken;
}


void GameEngine::MainPage::Button_Click_2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_rasterizer.RenderMeth(m_cube, XMMatrixIdentity(), m_viewMatrix, m_projectMatrix);

	m_bitmap->Invalidate();
}


void GameEngine::MainPage::Button_Click_3(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	m_rasterizer.RenderMeth(m_cube, XMMatrixIdentity(), m_viewMatrix, m_projectMatrix);

	m_bitmap->Invalidate();
}


void GameEngine::MainPage::Button_Click_4(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_rasterizer.RenderMeth(m_cube, XMMatrixIdentity(), m_viewMatrix, m_projectMatrix);

	m_bitmap->Invalidate();
}
