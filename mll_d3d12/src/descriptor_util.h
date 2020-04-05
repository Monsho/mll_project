#pragma once

#include "native.h"

#include <vector>
#include <memory>
#include <map>


namespace mll
{
	class Device;
	class CommandList;

	//-----------------------------------------------------------
	//! @brief descriptor stack heap.
	//-----------------------------------------------------------
	class DescriptorStackHeap
	{
		friend class ResourceDescriptorStack;
		friend class SamplerDescriptorStack;

	public:
		~DescriptorStackHeap();

		// getter
		ID3D12DescriptorHeap* GetNativeHeap()
		{
			return pHeap_;
		}
		u32 GetStackMax() const
		{
			return stackMax_;
		}

	private:
		DescriptorStackHeap()
		{}

		/**
		 * @brief initialize class.
		 *
		 * @param[in]		pDevice			parent device.
		 * @param[in]		stackSize		stack heap size.
		 * @return			initialize result.
		*/
		Result::Type Initialize(Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, u32 stackSize);

		/**
		 * @brief reset stack.
		*/
		void Reset()
		{
			stackPosition_ = 0;
		}

		/**
		 * @brief allocate from stack heap.
		 *
		 * @param[in]		count			alloc descriptor count.
		 * @param[out]		outCpu			alloc cpu handle.
		 * @param[out]		outGpu			alloc gpu handle.
		 * @return			allocate result.
		*/
		Result::Type Allocate(u32 count, D3D12_CPU_DESCRIPTOR_HANDLE& outCpu, D3D12_GPU_DESCRIPTOR_HANDLE& outGpu);

	private:
		ID3D12DescriptorHeap*		pHeap_ = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE	cpuHandleStart_{};
		D3D12_GPU_DESCRIPTOR_HANDLE	gpuHandleStart_{};
		u32							descSize_ = 0;
		u32							stackMax_ = 0;
		u32							stackPosition_ = 0;
	};	// class DescriptorStackHeap

	//-----------------------------------------------------------
	//! @brief descriptor stack for shader resource views.
	//-----------------------------------------------------------
	class ResourceDescriptorStack
	{
	public:
		ResourceDescriptorStack()
		{}
		~ResourceDescriptorStack();

		/**
		 * @brief initialize class.
		 *
		 * @param[in]		pDevice				parent device.
		 * @return			initialize result.
		*/
		Result::Type Initialize(Device* pDevice);

		/**
		 * @brief reset stack.
		*/
		void Reset();

		/**
		 * @brief allocate handle from stack and copy cpu handles.
		 *
		 * @param[in]		count				alloc descriptor count.
		 * @param[in]		pSrcCpu				copy source cpu handle.
		 * @param[out]		pOutCpu				alloc cpu handle. (nullptr ok)
		 * @param[out]		pOutGpu				alloc gpu handle. (nullptr ok)
		*/
		void AllocateAndCopy(u32 count, const D3D12_CPU_DESCRIPTOR_HANDLE* pSrcCpu, D3D12_CPU_DESCRIPTOR_HANDLE* pOutCpu, D3D12_GPU_DESCRIPTOR_HANDLE* pOutGpu);

		/**
		 * @brief mark heap dirty.
		*/
		void MarkHeapDirty()
		{
			heapDirty_ = true;
		}

		/**
		 * @brief unmark heap dirty.
		*/
		void UnmarkHeapDirty()
		{
			heapDirty_ = false;
		}

		// getter
		DescriptorStackHeap* GetCurrentHeap()
		{
			return pCurrentHeap_;
		}
		bool IsHeapDirty() const
		{
			return heapDirty_;
		}

	private:
		std::vector<std::unique_ptr<DescriptorStackHeap>>	heaps_;

		Device*					pParentDevice_ = nullptr;
		DescriptorStackHeap*	pCurrentHeap_ = nullptr;
		bool					heapDirty_ = false;
	};	// class ResourceDescriptorStack

	//-----------------------------------------------------------
	//! @brief descriptor stack for samplers.
	//-----------------------------------------------------------
	class SamplerDescriptorStack
	{
		struct MapItem
		{
			DescriptorStackHeap*		pHeap = nullptr;
			D3D12_CPU_DESCRIPTOR_HANDLE	cpuHandle;
			D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
		};	// struct MapItem

	public:
		SamplerDescriptorStack()
		{}
		~SamplerDescriptorStack();

		/**
		 * @brief initialize class.
		 *
		 * @param[in]		pDevice				parent device.
		 * @return			initialize result.
		*/
		Result::Type Initialize(Device* pDevice);

		/**
		 * @brief reset stack.
		*/
		void Reset();

		/**
		 * @brief allocate handle from stack and copy cpu handles.
		 *
		 * @param[in]		count				alloc descriptor count.
		 * @param[in]		pSrcCpu				copy source cpu handle.
		 * @param[out]		pOutCpu				alloc cpu handle. (nullptr ok)
		 * @param[out]		pOutGpu				alloc gpu handle. (nullptr ok)
		*/
		void AllocateAndCopy(u32 count, const D3D12_CPU_DESCRIPTOR_HANDLE* pSrcCpu, D3D12_CPU_DESCRIPTOR_HANDLE* pOutCpu, D3D12_GPU_DESCRIPTOR_HANDLE* pOutGpu);

		/**
		 * @brief mark heap dirty.
		*/
		void MarkHeapDirty()
		{
			heapDirty_ = true;
		}

		/**
		 * @brief unmark heap dirty.
		*/
		void UnmarkHeapDirty()
		{
			heapDirty_ = false;
		}

		// getter
		DescriptorStackHeap* GetCurrentHeap()
		{
			return pCurrentHeap_;
		}
		bool IsHeapDirty() const
		{
			return heapDirty_;
		}

	private:
		void AddHeap();

	private:
		std::vector<std::unique_ptr<DescriptorStackHeap>>	heaps_;

		Device*						pParentDevice_ = nullptr;
		DescriptorStackHeap*		pLastAllocateHeap_ = nullptr;
		DescriptorStackHeap*		pCurrentHeap_ = nullptr;
		bool						heapDirty_ = false;
		std::map<u32, MapItem>		caches_;
	};	// class SamplerDesctriptorStack

}
//	EOF
