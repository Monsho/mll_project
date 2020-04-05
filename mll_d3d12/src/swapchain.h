#pragma once

#include "native.h"


namespace mll
{
	class Device;

	//-----------------------------------------------------------
	//! @brief swapchain.
	//-----------------------------------------------------------
	class Swapchain
		: public ISwapchain
	{
		friend class IDevice;

	public:
		// getter
		NativeSwapchain* GetNativeSwapchain()
		{
			return pSwapchain_;
		}

	private:
		Swapchain()
			: ISwapchain()
		{}
		~Swapchain()
		{
			Destroy();
		}

		Result::Type Initialize(Device* pDevice, const SwapchainDesc& desc);
		void Destroy();

		/**
		 * @brief Release self.
		*/
		void Release() override;

	private:
		NativeSwapchain*		pSwapchain_ = nullptr;

		HANDLE		event_ = nullptr;
		u32			frameIndex_ = 0;
	};	// class Swapchain

}
//	EOF
