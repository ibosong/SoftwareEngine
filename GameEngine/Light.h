#pragma once
#include "directxmath.h"
namespace GameEngine
{
	class Light
	{
	public:
		Light(DirectX::XMFLOAT3 dotLight);
		inline DirectX::XMFLOAT3 GetDotLight(){ return m_dotLightPostion; }
	private:
		DirectX::XMFLOAT3 m_dotLightPostion;

	};
}


