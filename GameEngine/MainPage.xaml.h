//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "Rasterizer.h"
#include "DirectXMath.h"
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
		Texture m_texture;
		void OnRendering(Platform::Object^ sender, Platform::Object^ args);
		void OnPointerMoved(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args);
		Windows::Foundation::EventRegistrationToken m_pointerMoveToken;
		Windows::UI::Xaml::Media::Imaging::WriteableBitmap^ m_bitmap;
		DirectX::XMFLOAT2 Project(DirectX::XMVECTOR coord, DirectX::XMFLOAT4X4 transMatrix);
		unsigned int m_pixelWidth;
		unsigned int m_pixelHeight;

		DirectX::XMMATRIX m_viewMatrix;
		DirectX::XMMATRIX m_projectMatrix;
		
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Button_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		
		void Border_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Border_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Button_Click_2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Button_Click_3(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Button_Click_4(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};

	
}
