#include "swapchain.h"

#include <cassert>

#include "device.h"


namespace mll
{
	void Swapchain::Release()
	{
		KillSelf();
	}

	//-----------------------------------------------------------
	// initialize native command list.
	//-----------------------------------------------------------
	Result::Type Swapchain::Initialize(Device* pDevice, const SwapchainDesc& desc)
	{
		desc_ = desc;

		// create dxgi swapchain.
		{
			DXGI_SWAP_CHAIN_DESC1 sd = {};
			sd.BufferCount = desc.backBufferCount;
			sd.Width = desc.width;
			sd.Height = desc.height;
			sd.Format = GetNativeResourceFormat(desc.format);
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			sd.SampleDesc.Count = 1;
			sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

			IDXGISwapChain1* pSwap;
			auto hr = pDevice->GetNativeFactory()->CreateSwapChainForHwnd(pDevice->GetCommandQueue()->GetGraphicsQueue(), (HWND)desc.windowHandle, &sd, nullptr, nullptr, &pSwap);
			if (FAILED(hr))
			{
				return Result::InvalidArgs;
			}

			hr = pSwap->QueryInterface(IID_PPV_ARGS(&pSwapchain_));
			if (FAILED(hr))
			{
				return Result::InvalidArgs;
			}

			frameIndex_ = pSwapchain_->GetCurrentBackBufferIndex();
			event_ = pSwapchain_->GetFrameLatencyWaitableObject();

			pSwap->Release();
		}

		// create texture and view.

		return Result::Ok;
	}

	//-----------------------------------------------------------
	// destroy native command list.
	//-----------------------------------------------------------
	void Swapchain::Destroy()
	{
		SafeRelease(pSwapchain_);
	}


#define Self()	static_cast<Swapchain*>(this)

	//-----------------------------------------------------------
	// present swapchain.
	//-----------------------------------------------------------
	void ISwapchain::Present(u32 syncInterval)
	{
		auto p_native = Self()->GetNativeSwapchain();

		auto hr = p_native->Present(syncInterval, 0);
		assert(SUCCEEDED(hr));
	}

#undef Self
}
//	EOF
