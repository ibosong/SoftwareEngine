//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "Rasterizer.h"
#include "DirectXMath.h"
#include "BasicTimer.h"
namespace GameEngine
{

	public ref class MainPage sealed
	{
	public:
		MainPage();
	internal:
	
	private:
		Rasterizer m_rasterizer;
		byte* m_buffer;
		Mesh m_cube;

		BasicTimer^ m_timer;
		
		void OnRendering(Platform::Object^ sender, Platform::Object^ args);
		void OnPointerMoved(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args);
		Windows::Foundation::EventRegistrationToken m_pointerMoveToken;
		Windows::UI::Xaml::Media::Imaging::WriteableBitmap^ m_bitmap;
		DirectX::XMFLOAT2 Project(DirectX::XMVECTOR coord, DirectX::XMFLOAT4X4 transMatrix);
		unsigned int m_pixelWidth;
		unsigned int m_pixelHeight;

		DirectX::XMMATRIX m_viewMatrix;
		DirectX::XMMATRIX m_projectMatrix;
		
		void btnDraw3D_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnClear_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		
		void btnRenderZ_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnRenderX_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnRenderY_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Page_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void imageRendering_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void imageRendering_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
	};

	
}
