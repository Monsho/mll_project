#pragma once

#include "native.h"


namespace mll
{
	class Device;

	//-----------------------------------------------------------
	//! @brief Command queues.
	//-----------------------------------------------------------
	class CommandQueue
	{
	public:
		CommandQueue()
		{}
		~CommandQueue()
		{
			Destroy();
		}

		bool Initialize(Device* pDevice);
		void Destroy();

		ID3D12CommandQueue* GetGraphicsQueue()
		{
			return pGraphicsQueue_;
		}
		ID3D12CommandQueue* GetComputeQueue()
		{
			return (pComputeQueue_ != nullptr) ? pComputeQueue_ : pGraphicsQueue_;
		}
		ID3D12CommandQueue* GetCopyQueue()
		{
			return (pCopyQueue_ != nullptr) ? pCopyQueue_ : pGraphicsQueue_;
		}
		u64 GetTimestampFrequency() const
		{
			return timestampFrequency_;
		}

	private:
		ID3D12CommandQueue* pGraphicsQueue_ = nullptr;
		ID3D12CommandQueue* pComputeQueue_ = nullptr;
		ID3D12CommandQueue* pCopyQueue_ = nullptr;
		u64					timestampFrequency_ = 0;
	};	// class CommandQueue

	//-----------------------------------------------------------
	//! @brief Graphics device object.
	//-----------------------------------------------------------
	class Device
		: public IDevice
	{
		friend class IDevice;

	public:
		Device()
		{}
		~Device()
		{
			Destroy();
		}

		NativeFactory* GetNativeFactory()
		{
			return pFactory_;
		}
		NativeAdapter* GetNativeAdapter()
		{
			return pAdapter_;
		}
		NativeOutput* GetNativeOutput()
		{
			return pOutput_;
		}
		NativeDevice* GetNativeDevice()
		{
			return pDevice_;
		}
		CommandQueue* GetCommandQueue()
		{
			return pCommandQueue_;
		}

	private:
		bool Initialize(const DeviceDesc& desc);
		void Destroy();

	private:
		NativeFactory*		pFactory_ = nullptr;
		NativeAdapter*		pAdapter_ = nullptr;
		NativeOutput*		pOutput_ = nullptr;
		NativeDevice*		pDevice_ = nullptr;

		CommandQueue*		pCommandQueue_ = nullptr;
	};	// class Device

}
//	EOF
