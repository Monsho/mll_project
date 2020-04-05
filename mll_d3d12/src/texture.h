#pragma once

#include "native.h"


namespace mll
{
	class Device;

	//-----------------------------------------------------------
	//! @brief texture resource.
	//-----------------------------------------------------------
	class Texture
		: public ITexture
	{
		friend class IDevice;

	public:
		// getter
		ID3D12Resource* GetNativeTexture()
		{
			return pResource_;
		}

	private:
		Texture()
			: ITexture()
		{}
		~Texture()
		{
			Destroy();
		}

		Result::Type Initialize(Device* pDevice, const TextureDesc& desc);
		void Destroy();

		/**
		 * @brief Release self.
		*/
		void Release() override;

	private:
		ID3D12Resource*		pResource_ = nullptr;
	};	// class Texture

}
//	EOF
