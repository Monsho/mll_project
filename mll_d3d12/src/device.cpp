#include "device.h"

#include <cassert>

#include "command_list.h"


namespace mll
{
	//-----------------------------------------------------------
	// Initialize each command queue.
	//-----------------------------------------------------------
	bool CommandQueue::Initialize(Device* pDevice)
	{
		// create graphics queue.
		D3D12_COMMAND_QUEUE_DESC desc{};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;		// GPUタイムアウトが有効
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		auto hr = pDevice->GetNativeDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(&pGraphicsQueue_));
		if (FAILED(hr))
		{
			return false;
		}

		// create compute and copy queue.
		// if these queues is not created, it's ok.
		desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		hr = pDevice->GetNativeDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(&pComputeQueue_));
		if (FAILED(hr))
		{
			pComputeQueue_ = nullptr;
		}

		desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		hr = pDevice->GetNativeDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(&pCopyQueue_));
		if (FAILED(hr))
		{
			pCopyQueue_ = nullptr;
		}

		hr = pGraphicsQueue_->GetTimestampFrequency(&timestampFrequency_);
		if (FAILED(hr))
		{
			timestampFrequency_ = 0;
		}

		return true;
	}

	//-----------------------------------------------------------
	// Destroy each command queue.
	//-----------------------------------------------------------
	void CommandQueue::Destroy()
	{
		SafeRelease(pGraphicsQueue_);
		SafeRelease(pComputeQueue_);
		SafeRelease(pCopyQueue_);
	}


	//-----------------------------------------------------------
	// Release device.
	//-----------------------------------------------------------
	void IDevice::Release()
	{
		MLL_DELETE(this);
	}

	//-----------------------------------------------------------
	// Graphics device create function.
	//-----------------------------------------------------------
	ObjPtr<IDevice> IDevice::CreateGraphicsDevice(const DeviceDesc& desc)
	{
		auto ret = MLL_NEW(Device);

		auto init_result = ret->Initialize(desc);
		if (!init_result)
		{
			return ObjPtr<IDevice>();
		}

		return ObjPtr<IDevice>(ret, objectId_.fetch_add(1));
	}

	//-----------------------------------------------------------
	// Initialize device.
	//-----------------------------------------------------------
	bool Device::Initialize(const DeviceDesc& desc)
	{
#ifdef _DEBUG
		// デバッグレイヤーの有効化
		if (desc.enableDebugLayer)
		{
			ID3D12Debug* debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();
				debugController->Release();
			}
		}
#endif

		// ファクトリの生成
		IDXGIFactory2* p_factory;
		auto hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&p_factory));
		if (FAILED(hr))
		{
			return false;
		}
		hr = p_factory->QueryInterface(IID_PPV_ARGS(&pFactory_));
		SafeRelease(p_factory);
		if (FAILED(hr))
		{
			return false;
		}

		// アダプタを取得する
		bool is_warp = false;
		IDXGIAdapter1* p_adapter = nullptr;
		hr = pFactory_->EnumAdapters1(0, &p_adapter);
		if (FAILED(hr))
		{
			// 取得できない場合はWarpアダプタを取得
			SafeRelease(p_adapter);

			hr = pFactory_->EnumWarpAdapter(IID_PPV_ARGS(&p_adapter));
			if (FAILED(hr))
			{
				SafeRelease(p_adapter);
				return false;
			}
			is_warp = true;
		}
		hr = p_adapter->QueryInterface(IID_PPV_ARGS(&pAdapter_));
		SafeRelease(p_adapter);
		if (FAILED(hr))
		{
			return false;
		}

		if (!is_warp)
		{
			// ディスプレイを取得する
			IDXGIOutput* p_output = nullptr;
			hr = pAdapter_->EnumOutputs(0, &p_output);
			if (FAILED(hr))
			{
				return false;
			}

			hr = p_output->QueryInterface(IID_PPV_ARGS(&pOutput_));
			SafeRelease(p_output);
			if (FAILED(hr))
			{
				return false;
			}
		}

		// デバイスの生成
		ID3D12Device* p_device = nullptr;
		hr = D3D12CreateDevice(pAdapter_, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&p_device));
		if (FAILED(hr))
		{
			return false;
		}
		hr = p_device->QueryInterface(IID_PPV_ARGS(&pDevice_));
		SafeRelease(p_device);
		if (FAILED(hr))
		{
			return false;
		}

		// DirectX Raytracingが使用可能ならDXR用のデバイスを作成する
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupportData{};
		if (SUCCEEDED(pDevice_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureSupportData, sizeof(featureSupportData))))
		{
			enableRaytracing_ = featureSupportData.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;

			// COPY_DESCRIPTORS_INVALID_RANGESエラーを回避
			ID3D12InfoQueue* pD3DInfoQueue;
			if (SUCCEEDED(pDevice_->QueryInterface(__uuidof(ID3D12InfoQueue), reinterpret_cast<void**>(&pD3DInfoQueue))))
			{
#if 0
				// エラー等が出たときに止めたい場合は有効にする
				pD3DInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
				pD3DInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
				pD3DInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
#endif

				D3D12_MESSAGE_ID blockedIds[] = { D3D12_MESSAGE_ID_COPY_DESCRIPTORS_INVALID_RANGES };
				D3D12_INFO_QUEUE_FILTER filter = {};
				filter.DenyList.pIDList = blockedIds;
				filter.DenyList.NumIDs = _countof(blockedIds);
				pD3DInfoQueue->AddRetrievalFilterEntries(&filter);
				pD3DInfoQueue->AddStorageFilterEntries(&filter);
				pD3DInfoQueue->Release();
			}
		}

		// CommandQueue生成
		pCommandQueue_ = MLL_NEW(CommandQueue);
		assert(pCommandQueue_ != nullptr);
		if (!pCommandQueue_->Initialize(this))
		{
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------
	// Destroy device.
	//-----------------------------------------------------------
	void Device::Destroy()
	{
		u32 live_obj_cnt = IterateLiveObjects([](IDeviceChild* p) {});
		assert(live_obj_cnt == 0);

		ProcDeathList(true);

		MLL_DELETE(pCommandQueue_);

		SafeRelease(pDevice_);
		SafeRelease(pOutput_);
		SafeRelease(pAdapter_);
		SafeRelease(pFactory_);
	}


	//-----------------------------------------------------------
	// Create command list.
	//-----------------------------------------------------------
	Result::Type IDevice::CreateCommandList(const CommandListDesc& desc, ObjPtr<ICommandList>& outObj)
	{
		auto p = MLL_NEW(CommandList);

		auto result = p->Initialize(static_cast<Device*>(this), desc);
		if (IsFailed(result))
		{
			MLL_DELETE(p);
			return result;
		}

		outObj = AppendDeviceChild<ICommandList>(p);
		return Result::Ok;
	}

}
//	EOF
