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

MainPage::MainPage() : m_pixelWidth(1000), m_pixelHeight(1000), m_timer(ref new BasicTimer)
{
	InitializeComponent();	

	CompositionTarget::Rendering += ref new EventHandler<Object^>(this, &MainPage::OnRendering);	
}


void GameEngine::MainPage::Page_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_pixelWidth = this->ActualWidth;
	m_pixelHeight = this->ActualHeight;

	//std::vector<Vertex> vertices = {
	//	// +z face
	//	{ XMFLOAT3(-0.3f, -0.3f, 0.3f), XMFLOAT3(0.f, 0.f, 1.f), Color(1.f, 0.f, 0.f), XMFLOAT2(0.f, 0.f) },
	//	{ XMFLOAT3(0.3f, -0.3f, 0.3f), XMFLOAT3(0.f, 0.f, 1.f), Color(1.f, 0.f, 0.f), XMFLOAT2(0.f, 1.f) },
	//	{ XMFLOAT3(0.3f, 0.3f, 0.3f), XMFLOAT3(0.f, 0.f, 1.f), Color(1.f, 0.f, 0.f), XMFLOAT2(1.f, 0.f) },
	//	{ XMFLOAT3(-0.3f, 0.3f, 0.3f), XMFLOAT3(0.f, 0.f, 1.f), Color(1.f, 0.f, 0.f), XMFLOAT2(1.f, 1.f) },
	//	// -z face
	//	{ XMFLOAT3(-0.3f, -0.3f, -0.3f), XMFLOAT3(0.f, 0.f, -1.f), Color(1.f, 1.f, 0.f), XMFLOAT2(0.f, 0.f) },
	//	{ XMFLOAT3(-0.3f, 0.3f, -0.3f), XMFLOAT3(0.f, 0.f, -1.f), Color(1.f, 1.f, 0.f), XMFLOAT2(0.f, 1.f) },
	//	{ XMFLOAT3(0.3f, 0.3f, -0.3f), XMFLOAT3(0.f, 0.f, -1.f), Color(1.f, 1.f, 0.f), XMFLOAT2(1.f, 0.f) },
	//	{ XMFLOAT3(0.3f, -0.3f, -0.3f), XMFLOAT3(0.f, 0.f, -1.f), Color(1.f, 1.f, 0.f), XMFLOAT2(1.f, 1.f) },
	//	// +y face
	//	{ XMFLOAT3(-0.3f, 0.3f, -0.3f), XMFLOAT3(0.f, 1.f, 0.f), Color(0.f, 1.f, 0.f), XMFLOAT2(0.f, 0.f) },
	//	{ XMFLOAT3(-0.3f, 0.3f, 0.3f), XMFLOAT3(0.f, 1.f, 0.f), Color(0.f, 1.f, 0.f), XMFLOAT2(0.f, 1.f) },
	//	{ XMFLOAT3(0.3f, 0.3f, 0.3f), XMFLOAT3(0.f, 1.f, 0.f), Color(0.f, 1.f, 0.f), XMFLOAT2(1.f, 0.f) },
	//	{ XMFLOAT3(0.3f, 0.3f, -0.3f), XMFLOAT3(0.f, 1.f, 0.f), Color(0.f, 1.f, 0.f), XMFLOAT2(1.f, 1.f) },
	//	// -y face
	//	{ XMFLOAT3(-0.3f, -0.3f, -0.3f), XMFLOAT3(0.f, -1.f, 0.f), Color(1.f, 0.f, 1.f), XMFLOAT2(0.f, 0.f) },
	//	{ XMFLOAT3(0.3f, -0.3f, -0.3f), XMFLOAT3(0.f, -1.f, 0.f), Color(1.f, 0.f, 1.f), XMFLOAT2(0.f, 1.f) },
	//	{ XMFLOAT3(0.3f, -0.3f, 0.3f), XMFLOAT3(0.f, -1.f, 0.f), Color(1.f, 0.f, 1.f), XMFLOAT2(1.f, 0.f) },
	//	{ XMFLOAT3(-0.3f, -0.3f, 0.3f), XMFLOAT3(0.f, -1.f, 0.f), Color(1.f, 0.f, 1.f), XMFLOAT2(1.f, 1.f) },
	//	// -x face
	//	{ XMFLOAT3(-0.3f, -0.3f, 0.3f), XMFLOAT3(-1.f, 0.f, 0.f), Color(0.f, 0.f, 1.f), XMFLOAT2(0.f, 0.f) },
	//	{ XMFLOAT3(-0.3f, 0.3f, 0.3f), XMFLOAT3(-1.f, 0.f, 0.f), Color(0.f, 0.f, 1.f), XMFLOAT2(0.f, 1.f) },
	//	{ XMFLOAT3(-0.3f, 0.3f, -0.3f), XMFLOAT3(-1.f, 0.f, 0.f), Color(0.f, 0.f, 1.f), XMFLOAT2(1.f, 0.f) },
	//	{ XMFLOAT3(-0.3f, -0.3f, -0.3f), XMFLOAT3(-1.f, 0.f, 0.f), Color(0.f, 0.f, 1.f), XMFLOAT2(1.f, 1.f) },
	//	// +x face
	//	{ XMFLOAT3(0.3f, -0.3f, -0.3f), XMFLOAT3(1.f, 0.f, 0.f), Color(0.f, 1.f, 1.f), XMFLOAT2(0.f, 0.f) },
	//	{ XMFLOAT3(0.3f, 0.3f, -0.3f), XMFLOAT3(1.f, 0.f, 0.f), Color(0.f, 1.f, 1.f), XMFLOAT2(0.f, 1.f) },
	//	{ XMFLOAT3(0.3f, 0.3f, 0.3f), XMFLOAT3(1.f, 0.f, 0.f), Color(0.f, 1.f, 1.f), XMFLOAT2(1.f, 0.f) },
	//	{ XMFLOAT3(0.3f, -0.3f, 0.3f), XMFLOAT3(1.f, 0.f, 0.f), Color(0.f, 1.f, 1.f), XMFLOAT2(1.f, 1.f) }
	//};

	// No colors
	std::vector<Vertex> vertices = {
		// +z face
		{ XMFLOAT3(-0.3f, -0.3f, 0.3f), XMFLOAT3(0.f, 0.f, 1.f), Color(1.f, 1.f, 1.f), XMFLOAT2(0.f, 0.f) },
		{ XMFLOAT3(0.3f, -0.3f, 0.3f), XMFLOAT3(0.f, 0.f, 1.f), Color(1.f, 1.f, 1.f), XMFLOAT2(0.f, 1.f) },
		{ XMFLOAT3(0.3f, 0.3f, 0.3f), XMFLOAT3(0.f, 0.f, 1.f), Color(1.f, 1.f, 1.f), XMFLOAT2(1.f, 0.f) },
		{ XMFLOAT3(-0.3f, 0.3f, 0.3f), XMFLOAT3(0.f, 0.f, 1.f), Color(1.f, 1.f, 1.f), XMFLOAT2(1.f, 1.f) },
		// -z face
		{ XMFLOAT3(-0.3f, -0.3f, -0.3f), XMFLOAT3(0.f, 0.f, -1.f), Color(1.f, 1.f, 1.f), XMFLOAT2(0.f, 0.f) },
		{ XMFLOAT3(-0.3f, 0.3f, -0.3f), XMFLOAT3(0.f, 0.f, -1.f), Color(1.f, 1.f, 1.f), XMFLOAT2(0.f, 1.f) },
		{ XMFLOAT3(0.3f, 0.3f, -0.3f), XMFLOAT3(0.f, 0.f, -1.f), Color(1.f, 1.f, 1.f), XMFLOAT2(1.f, 0.f) },
		{ XMFLOAT3(0.3f, -0.3f, -0.3f), XMFLOAT3(0.f, 0.f, -1.f), Color(1.f, 1.f, 1.f), XMFLOAT2(1.f, 1.f) },
		// +y face
		{ XMFLOAT3(-0.3f, 0.3f, -0.3f), XMFLOAT3(0.f, 1.f, 0.f), Color(1.f, 1.f, 1.f), XMFLOAT2(0.f, 0.f) },
		{ XMFLOAT3(-0.3f, 0.3f, 0.3f), XMFLOAT3(0.f, 1.f, 0.f), Color(1.f, 1.f, 1.f), XMFLOAT2(0.f, 1.f) },
		{ XMFLOAT3(0.3f, 0.3f, 0.3f), XMFLOAT3(0.f, 1.f, 0.f), Color(1.f, 1.f, 1.f), XMFLOAT2(1.f, 0.f) },
		{ XMFLOAT3(0.3f, 0.3f, -0.3f), XMFLOAT3(0.f, 1.f, 0.f), Color(1.f, 1.f, 1.f), XMFLOAT2(1.f, 1.f) },
		// -y face
		{ XMFLOAT3(-0.3f, -0.3f, -0.3f), XMFLOAT3(0.f, -1.f, 0.f), Color(1.f, 0.f, 1.f), XMFLOAT2(0.f, 0.f) },
		{ XMFLOAT3(0.3f, -0.3f, -0.3f), XMFLOAT3(0.f, -1.f, 0.f), Color(1.f, 0.f, 1.f), XMFLOAT2(0.f, 1.f) },
		{ XMFLOAT3(0.3f, -0.3f, 0.3f), XMFLOAT3(0.f, -1.f, 0.f), Color(1.f, 0.f, 1.f), XMFLOAT2(1.f, 0.f) },
		{ XMFLOAT3(-0.3f, -0.3f, 0.3f), XMFLOAT3(0.f, -1.f, 0.f), Color(1.f, 0.f, 1.f), XMFLOAT2(1.f, 1.f) },
		// -x face
		{ XMFLOAT3(-0.3f, -0.3f, 0.3f), XMFLOAT3(-1.f, 0.f, 0.f), Color(1.f, 1.f, 1.f), XMFLOAT2(0.f, 0.f) },
		{ XMFLOAT3(-0.3f, 0.3f, 0.3f), XMFLOAT3(-1.f, 0.f, 0.f), Color(1.f, 1.f, 1.f), XMFLOAT2(0.f, 1.f) },
		{ XMFLOAT3(-0.3f, 0.3f, -0.3f), XMFLOAT3(-1.f, 0.f, 0.f), Color(1.f, 1.f, 1.f), XMFLOAT2(1.f, 0.f) },
		{ XMFLOAT3(-0.3f, -0.3f, -0.3f), XMFLOAT3(-1.f, 0.f, 0.f), Color(1.f, 1.f, 1.f), XMFLOAT2(1.f, 1.f) },
		// +x face
		{ XMFLOAT3(0.3f, -0.3f, -0.3f), XMFLOAT3(1.f, 0.f, 0.f), Color(1.f, 1.f, 1.f), XMFLOAT2(0.f, 0.f) },
		{ XMFLOAT3(0.3f, 0.3f, -0.3f), XMFLOAT3(1.f, 0.f, 0.f), Color(1.f, 1.f, 1.f), XMFLOAT2(0.f, 1.f) },
		{ XMFLOAT3(0.3f, 0.3f, 0.3f), XMFLOAT3(1.f, 0.f, 0.f), Color(1.f, 1.f, 1.f), XMFLOAT2(1.f, 0.f) },
		{ XMFLOAT3(0.3f, -0.3f, 0.3f), XMFLOAT3(1.f, 0.f, 0.f), Color(1.f, 1.f, 1.f), XMFLOAT2(1.f, 1.f) }
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

	Texture::Load("image.jpg").then([this, vertices, cubeIndices](std::shared_ptr<Texture> texture){
		m_cube = { vertices, cubeIndices, nullptr };
	});


	m_skullModel = CMOModel::CreateFromCMO(L"skull.cmo");

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
	static const XMVECTORF32 eye = { 0.0f, 0.9f, 13.5f, 0.0f };
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
	Light light = Light(XMFLOAT3(10.f, 12.f, 12.f));
	m_rasterizer.SetLight(light);

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

	//m_rasterizer.RenderMeth(m_cube, world, m_viewMatrix, m_projectMatrix);
	m_rasterizer.RenderCMOModel(m_skullModel.get(), world, m_viewMatrix, m_projectMatrix);

	m_bitmap->Invalidate();
	if (m_timer->Delta > 0.f)
		fpsTextBlock->Text = (1.f / m_timer->Delta).ToString();
	m_timer->Update();

}

void MainPage::btnDraw3D_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_rasterizer.RenderMeth(m_cube, XMMatrixIdentity(), m_viewMatrix, m_projectMatrix);

	m_bitmap->Invalidate();
}


void MainPage::btnClear_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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


void MainPage::btnRenderZ_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_rasterizer.RenderZ(m_cube, XMMatrixIdentity(), m_viewMatrix, m_projectMatrix);

	m_bitmap->Invalidate();
}


void MainPage::btnRenderX_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	m_rasterizer.RenderX(m_cube, XMMatrixIdentity(), m_viewMatrix, m_projectMatrix);

	m_bitmap->Invalidate();
}


void MainPage::btnRenderY_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_rasterizer.RenderY(m_cube, XMMatrixIdentity(), m_viewMatrix, m_projectMatrix);

	m_bitmap->Invalidate();
}




void MainPage::imageRendering_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_pointerMoveToken = Window::Current->CoreWindow->PointerMoved +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &MainPage::OnPointerMoved);

}


void MainPage::imageRendering_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	Window::Current->CoreWindow->PointerMoved -= m_pointerMoveToken;
}
