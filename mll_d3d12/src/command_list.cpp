#include "command_list.h"

#include <cassert>

#include "device.h"


namespace mll
{
	void CommandList::Release()
	{
		KillSelf();
	}

	//-----------------------------------------------------------
	// initialize native command list.
	//-----------------------------------------------------------
	Result::Type CommandList::Initialize(Device* pDevice, const CommandListDesc& desc)
	{
		desc_ = desc;

		// create command allocator.
		auto native_device = pDevice->GetNativeDevice();
		auto native_type = GetNativeCommandListType(desc.typeCommandQueue);
		auto hr = native_device->CreateCommandAllocator(native_type, IID_PPV_ARGS(&pCmdAllocator_));
		if (FAILED(hr))
		{
			return Result::InvalidArgs;
		}

		// create command list.
		ID3D12CommandList* cmd_list_base;
		hr = native_device->CreateCommandList(GetNodeMask(), native_type, pCmdAllocator_, nullptr, IID_PPV_ARGS(&cmd_list_base));
		if (FAILED(hr))
		{
			return Result::InvalidArgs;
		}
		hr = cmd_list_base->QueryInterface(IID_PPV_ARGS(&pCmdList_));
		if (FAILED(hr))
		{
			return Result::InvalidArgs;
		}
		SafeRelease(cmd_list_base);

		pCmdList_->Close();

		// create descriptor stack.
		if (desc.typeCommandQueue == CommandQueueType::Graphics || desc.typeCommandQueue == CommandQueueType::Compute)
		{
			pResourceDescriptorStack_ = std::make_unique<ResourceDescriptorStack>();
			auto result = pResourceDescriptorStack_->Initialize(pDevice);
			assert(IsSucceeded(result));

			pSamplerDescriptorStack_ = std::make_unique<SamplerDescriptorStack>();
			result = pSamplerDescriptorStack_->Initialize(pDevice);
			assert(IsSucceeded(result));
		}

		return Result::Ok;
	}

	//-----------------------------------------------------------
	// destroy native command list.
	//-----------------------------------------------------------
	void CommandList::Destroy()
	{
		pSamplerDescriptorStack_.reset(nullptr);
		pResourceDescriptorStack_.reset(nullptr);
		SafeRelease(pCmdList_);
		SafeRelease(pCmdAllocator_);
	}

	//-----------------------------------------------------------
	// set descriptor heaps to command list.
	//-----------------------------------------------------------
	void CommandList::SetDescriptorHeap()
	{
		if (desc_.typeCommandQueue == CommandQueueType::Graphics || desc_.typeCommandQueue == CommandQueueType::Compute)
		{
			if (pResourceDescriptorStack_->IsHeapDirty() || pSamplerDescriptorStack_->IsHeapDirty())
			{
				ID3D12DescriptorHeap* p_heaps[] = {
					pResourceDescriptorStack_->GetCurrentHeap()->GetNativeHeap(),
					pSamplerDescriptorStack_->GetCurrentHeap()->GetNativeHeap(),
				};
				pCmdList_->SetDescriptorHeaps(ARRAYSIZE(p_heaps), p_heaps);

				pResourceDescriptorStack_->UnmarkHeapDirty();
				pSamplerDescriptorStack_->UnmarkHeapDirty();
			}
		}
	}


#define Self()	static_cast<CommandList*>(this)

	//-----------------------------------------------------------
	// begin command load.
	//-----------------------------------------------------------
	void ICommandList::Begin()
	{
		auto p_this = Self();

		auto hr = p_this->GetNativeCmdAllocator()->Reset();
		assert(SUCCEEDED(hr));

		hr = p_this->GetNativeCmdList()->Reset(p_this->GetNativeCmdAllocator(), nullptr);
		assert(SUCCEEDED(hr));

		if (desc_.typeCommandQueue == CommandQueueType::Graphics || desc_.typeCommandQueue == CommandQueueType::Compute)
		{
			p_this->GetResourceDescriptorStack()->Reset();
			p_this->GetSamplerDescriptorStack()->Reset();
		}
	}

	//-----------------------------------------------------------
	// end command load.
	//-----------------------------------------------------------
	void ICommandList::End()
	{
		auto p_this = Self();
		auto hr = p_this->GetNativeCmdList()->Close();
		assert(SUCCEEDED(hr));
	}

#undef Self
}
//	EOF
