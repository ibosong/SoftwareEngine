#include "pch.h"
#include "Texture.h"
#include "Robuffer.h"

using namespace GameEngine;
using namespace concurrency;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Platform;


task<std::shared_ptr<Texture>> Texture::Load(String^ filename)
{	
	return create_task(Windows::ApplicationModel::Package::Current->InstalledLocation->GetFileAsync(filename))
		.then([&](StorageFile^ file){
		return create_task(file->OpenAsync(FileAccessMode::Read)).then([=](IRandomAccessStream^ stream){
			return create_task(file->Properties->GetImagePropertiesAsync()).then([=](FileProperties::ImageProperties^ prop){
				unsigned int width = prop->Width;
				unsigned int height = prop->Height;
				WriteableBitmap^ bitmap = ref new WriteableBitmap(width, height);
				bitmap->SetSource(stream);

				// Query the IBufferByteAccess interface.
				Microsoft::WRL::ComPtr<IBufferByteAccess> bufferByteAccess;
				reinterpret_cast<IInspectable*>(bitmap->PixelBuffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));

				// Retrieve the buffer data.
				byte* tempBuffer = nullptr;
				HRESULT hr = bufferByteAccess->Buffer(&tempBuffer);
				if (FAILED(hr))
				{
					throw Exception::CreateException(hr);
				}

				byte* buffer = new byte[width * height * 4];
				memset(buffer, 0, width * height * 4);
				memcpy(buffer, tempBuffer, width * height * 4);

				return std::make_shared<Texture>(Texture(buffer, width, height));
			});
		});
	});
}



Texture::Texture(byte* buffer, int width, int height) : m_buffer(buffer), m_width(width), m_height(height)
{

}

Color Texture::Map(float u, float v)
{
	if (m_buffer == nullptr)
	{
		return Color();
	}
	unsigned int iu = std::abs(static_cast<int>(u * m_width) % (m_width + 1));
	unsigned int iv = std::abs(static_cast<int>(v * m_height) % (m_height + 1));

	int pos = static_cast<int>(iu + iv * m_width) * 4;

	byte B = m_buffer[pos];
	byte G = m_buffer[pos + 1];
	byte R = m_buffer[pos + 2];
	byte A = m_buffer[pos + 3];

	return Color(R / 255.f, G / 255.f, B / 255.f, A / 255.f);
	
}