#pragma once

#include "native.h"

#include "descriptor_util.h"


namespace mll
{
	class Device;

	//-----------------------------------------------------------
	//! @brief command list.
	//-----------------------------------------------------------
	class CommandList
		: public ICommandList
	{
		friend class IDevice;

	public:
		/**
		 * @brief set descriptor heaps to command list.
		*/
		void SetDescriptorHeap();

		// getter
		ID3D12CommandAllocator* GetNativeCmdAllocator()
		{
			return pCmdAllocator_;
		}
		NativeCommandList* GetNativeCmdList()
		{
			return pCmdList_;
		}
		std::unique_ptr<ResourceDescriptorStack>& GetResourceDescriptorStack()
		{
			return pResourceDescriptorStack_;
		}
		std::unique_ptr<SamplerDescriptorStack>& GetSamplerDescriptorStack()
		{
			return pSamplerDescriptorStack_;
		}

	private:
		CommandList()
			: ICommandList()
		{}
		~CommandList()
		{
			Destroy();
		}

		Result::Type Initialize(Device* pDevice, const CommandListDesc& desc);
		void Destroy();

		/**
		 * @brief Release self.
		*/
		void Release() override;

	private:
		ID3D12CommandAllocator*		pCmdAllocator_ = nullptr;
		NativeCommandList*			pCmdList_ = nullptr;

		std::unique_ptr<ResourceDescriptorStack>	pResourceDescriptorStack_;
		std::unique_ptr<SamplerDescriptorStack>		pSamplerDescriptorStack_;
	};	// class CommandList

}
//	EOF
