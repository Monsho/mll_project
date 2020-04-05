#include "texture.h"

#include <cassert>

#include "device.h"


namespace mll
{
	void Texture::Release()
	{
		KillSelf();
	}

	//-----------------------------------------------------------
	// initialize native command list.
	//-----------------------------------------------------------
	Result::Type Texture::Initialize(Device* pDevice, const TextureDesc& desc)
	{
		desc_ = desc;

		if (desc.usageFlags & (ResourceUsageFlag::ConstantBuffer | ResourceUsageFlag::IndexBuffer | ResourceUsageFlag::VertexBuffer | ResourceUsageFlag::IndirectArg))
		{
			return Result::InvalidArgs;
		}

		// if heap is default, create texture resource.
		if (desc.heap == ResourceHeap::Default)
		{
			bool is_render_target = desc.usageFlags & ResourceUsageFlag::RenderTarget;
			bool is_depth_stencil = desc.usageFlags & ResourceUsageFlag::DepthStencil;
			bool is_uav = desc.usageFlags & ResourceUsageFlag::UnorderedAccess;

			D3D12_HEAP_PROPERTIES prop{};
			prop.Type = D3D12_HEAP_TYPE_DEFAULT;
			prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			prop.CreationNodeMask = GetNodeMask();
			prop.VisibleNodeMask = GetNodeMask();

			D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE;

			// consider depth format.
			auto format = GetNativeResourceFormat(desc.format);
			switch (format)
			{
			case DXGI_FORMAT_D32_FLOAT:
				format = DXGI_FORMAT_R32_TYPELESS; break;
			case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
				format = DXGI_FORMAT_R32G8X24_TYPELESS; break;
			case DXGI_FORMAT_D24_UNORM_S8_UINT:
				format = DXGI_FORMAT_R24G8_TYPELESS; break;
			case DXGI_FORMAT_D16_UNORM:
				format = DXGI_FORMAT_R16_TYPELESS; break;
			}

			D3D12_RESOURCE_DESC rd{};
			rd.Dimension = GetNativeResourceDimension(desc.dimension);
			rd.Alignment = 0;
			rd.Width = desc.width;
			rd.Height = desc.height;
			rd.DepthOrArraySize = (desc.dimension == ResourceDimension::Texture3D) ? desc.depth : desc.arraySize;
			rd.MipLevels = desc.mipLevels;
			rd.Format = format;
			rd.SampleDesc.Count = desc.sampleCount;
			rd.SampleDesc.Quality = 0;
			rd.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			rd.Flags = is_render_target ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_NONE;
			rd.Flags |= is_depth_stencil ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAG_NONE;
			rd.Flags |= is_uav ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE;

			auto hr = pDevice->GetNativeDevice()->CreateCommittedResource(&prop, flags, &rd, GetNativeResourceState(desc.initialState), nullptr, IID_PPV_ARGS(&pResource_));
			if (FAILED(hr))
			{
				return Result::InvalidOperation;
			}
		}

		return Result::Ok;
	}

	//-----------------------------------------------------------
	// destroy native command list.
	//-----------------------------------------------------------
	void Texture::Destroy()
	{
		SafeRelease(pResource_);
	}


#define Self()	static_cast<Texture*>(this)


#undef Self
}
//	EOF
