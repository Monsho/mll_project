#include "descriptor_util.h"

#include <cassert>
#include <algorithm>

#include "device.h"
#include "command_list.h"


namespace mll
{
	namespace
	{
		static const u32	kDefaultResourceHeapSize = 2048;
		static const u32	kMaxSamplerHeapSize = 2048;
	}

	//-----------------------------------------------------------
	// destructor for descriptor stack heap.
	//-----------------------------------------------------------
	DescriptorStackHeap::~DescriptorStackHeap()
	{
		SafeRelease(pHeap_);
	}

	//-----------------------------------------------------------
	// initialize descriptor stack heap.
	//-----------------------------------------------------------
	Result::Type DescriptorStackHeap::Initialize(Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, u32 stackSize)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.Type = type;
		desc.NumDescriptors = stackSize;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = GetNodeMask();

		auto hr = pDevice->GetNativeDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pHeap_));
		if (FAILED(hr))
		{
			return Result::OutOfMemory;
		}

		cpuHandleStart_ = pHeap_->GetCPUDescriptorHandleForHeapStart();
		gpuHandleStart_ = pHeap_->GetGPUDescriptorHandleForHeapStart();
		descSize_ = pDevice->GetNativeDevice()->GetDescriptorHandleIncrementSize(desc.Type);
		stackMax_ = stackSize;
		stackPosition_ = 0;

		return Result::Ok;
	}

	//-----------------------------------------------------------
	// allocate from stack heap.
	//-----------------------------------------------------------
	Result::Type DescriptorStackHeap::Allocate(u32 count, D3D12_CPU_DESCRIPTOR_HANDLE& outCpu, D3D12_GPU_DESCRIPTOR_HANDLE& outGpu)
	{
		if (stackPosition_ + count < stackMax_)
		{
			return Result::OutOfMemory;
		}

		outCpu = cpuHandleStart_;
		outGpu = gpuHandleStart_;
		outCpu.ptr += ((SIZE_T)stackPosition_ * (SIZE_T)descSize_);
		outGpu.ptr += ((u64)stackPosition_ * (u64)descSize_);
		stackPosition_ += count;

		return Result::Ok;
	}


	//-----------------------------------------------------------
	// destructor for resource descriptor stack.
	//-----------------------------------------------------------
	ResourceDescriptorStack::~ResourceDescriptorStack()
	{
		heaps_.clear();
	}

	//-----------------------------------------------------------
	// initialize resource descriptor stack.
	//-----------------------------------------------------------
	Result::Type ResourceDescriptorStack::Initialize(Device* pDevice)
	{
		assert(pDevice != nullptr);

		pParentDevice_ = pDevice;

		return Result::Ok;
	}

	//-----------------------------------------------------------
	// reset stack.
	//-----------------------------------------------------------
	void ResourceDescriptorStack::Reset()
	{
		u32 stack_size = kDefaultResourceHeapSize;

		if (heaps_.size() > 1)
		{
			stack_size = heaps_[0]->GetStackMax() * (u32)heaps_.size();
			heaps_.clear();
		}

		if (heaps_.empty())
		{
			std::unique_ptr<DescriptorStackHeap> heap(new DescriptorStackHeap());
			auto result = heap->Initialize(pParentDevice_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, stack_size);
			assert(IsSucceeded(result));

			heaps_.push_back(std::move(heap));
		}

		pCurrentHeap_ = heaps_[0].get();
		pCurrentHeap_->Reset();
		MarkHeapDirty();
	}

	//-----------------------------------------------------------
	// allocate handle from stack and copy cpu handles.
	//-----------------------------------------------------------
	void ResourceDescriptorStack::AllocateAndCopy(u32 count, const D3D12_CPU_DESCRIPTOR_HANDLE* pSrcCpu, D3D12_CPU_DESCRIPTOR_HANDLE* pOutCpu, D3D12_GPU_DESCRIPTOR_HANDLE* pOutGpu)
	{
		// try allocate from current heap.
		D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle;
		auto result = pCurrentHeap_->Allocate(count, cpu_handle, gpu_handle);
		if (IsFailed(result))
		{
			// create new heap because current heap is lack!
			std::unique_ptr<DescriptorStackHeap> heap(new DescriptorStackHeap());
			auto result = heap->Initialize(pParentDevice_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, std::max(pCurrentHeap_->GetStackMax(), count));
			assert(IsSucceeded(result));

			pCurrentHeap_ = heap.get();
			heaps_.push_back(std::move(heap));
			MarkHeapDirty();

			// allocater from current heap.
			result = pCurrentHeap_->Allocate(count, cpu_handle, gpu_handle);
			assert(IsSucceeded(result));
		}

		// copy descriptors.
		pParentDevice_->GetNativeDevice()->CopyDescriptors(
			1, &cpu_handle, &count,
			count, pSrcCpu, nullptr,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		if (pOutCpu) *pOutCpu = cpu_handle;
		if (pOutGpu) *pOutGpu = gpu_handle;
	}


	//-----------------------------------------------------------
	// destructor for sampler descriptor stack.
	//-----------------------------------------------------------
	SamplerDescriptorStack::~SamplerDescriptorStack()
	{
		heaps_.clear();
	}

	//-----------------------------------------------------------
	// add descriptor stack heap.
	//-----------------------------------------------------------
	void SamplerDescriptorStack::AddHeap()
	{
		std::unique_ptr<DescriptorStackHeap> heap(new DescriptorStackHeap());
		auto result = heap->Initialize(pParentDevice_, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, kMaxSamplerHeapSize);
		assert(IsSucceeded(result));

		pLastAllocateHeap_ = heap.get();
		heaps_.push_back(std::move(heap));
	}

	//-----------------------------------------------------------
	// initialize resource descriptor stack.
	//-----------------------------------------------------------
	Result::Type SamplerDescriptorStack::Initialize(Device* pDevice)
	{
		assert(pDevice != nullptr);

		pParentDevice_ = pDevice;
		AddHeap();

		return Result::Ok;
	}

	//-----------------------------------------------------------
	// reset stack.
	//-----------------------------------------------------------
	void SamplerDescriptorStack::Reset()
	{
		pCurrentHeap_ = nullptr;
		MarkHeapDirty();
	}

	//-----------------------------------------------------------
	// allocate handle from stack and copy cpu handles.
	//-----------------------------------------------------------
	void SamplerDescriptorStack::AllocateAndCopy(u32 count, const D3D12_CPU_DESCRIPTOR_HANDLE* pSrcCpu, D3D12_CPU_DESCRIPTOR_HANDLE* pOutCpu, D3D12_GPU_DESCRIPTOR_HANDLE* pOutGpu)
	{
		// calc hash.
		u32 hash = CalcFnv1a32(pSrcCpu, sizeof(pSrcCpu[0]) * count);

		// find cache.
		auto it = caches_.find(hash);
		if (it != caches_.end())
		{
			if (pOutCpu) *pOutCpu = it->second.cpuHandle;
			if (pOutGpu) *pOutGpu = it->second.gpuHandle;
			heapDirty_ = (pCurrentHeap_ != it->second.pHeap);
			pCurrentHeap_ = it->second.pHeap;
			return;
		}

		// try allocate from last allocate heap.
		D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle;
		auto result = pLastAllocateHeap_->Allocate(count, cpu_handle, gpu_handle);
		if (IsFailed(result))
		{
			// create new heap because current heap is lack!
			AddHeap();

			// allocater from last allocate heap.
			result = pLastAllocateHeap_->Allocate(count, cpu_handle, gpu_handle);
			assert(IsSucceeded(result));
		}

		heapDirty_ = (pCurrentHeap_ != pLastAllocateHeap_);
		pCurrentHeap_ = pLastAllocateHeap_;

		// copy descriptors.
		pParentDevice_->GetNativeDevice()->CopyDescriptors(
			1, &cpu_handle, &count,
			count, pSrcCpu, nullptr,
			D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

		if (pOutCpu) *pOutCpu = cpu_handle;
		if (pOutGpu) *pOutGpu = gpu_handle;
	}

}
//	EOF
