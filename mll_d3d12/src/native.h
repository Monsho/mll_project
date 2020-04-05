﻿#pragma once

#define NOMINMAX

#include <d3d12.h>
#include <dxgi1_6.h>

#include "mll/mll_defines.h"
#include "mll/mll_interfaces.h"


namespace mll
{
	static const UINT	kNodeMaskDefault = 0x01;

	typedef IDXGIFactory7					NativeFactory;
	typedef IDXGIAdapter4					NativeAdapter;
	typedef IDXGIOutput6					NativeOutput;
	typedef ID3D12Device6					NativeDevice;
	typedef ID3D12GraphicsCommandList5		NativeCommandList;
	typedef IDXGISwapChain4					NativeSwapchain;

	/**
	 * @brief get node mask.
	*/
	inline UINT GetNodeMask()
	{
		return kNodeMaskDefault;
	}

	/**
	 * @brief get d3d12 command queue type.
	*/
	inline D3D12_COMMAND_LIST_TYPE GetNativeCommandListType(CommandQueueType::Type v)
	{
		static const D3D12_COMMAND_LIST_TYPE k[] = {
			D3D12_COMMAND_LIST_TYPE_DIRECT,			// Graphics
			D3D12_COMMAND_LIST_TYPE_COMPUTE,		// Compute
			D3D12_COMMAND_LIST_TYPE_COPY,			// Copy
		};
		return k[v];
	}

	/**
	 * @brief get dxgi format.
	*/
	inline DXGI_FORMAT GetNativeResourceFormat(ResourceFormat::Type v)
	{
		static const DXGI_FORMAT k[] = {
			DXGI_FORMAT_UNKNOWN,		// Unknown
			DXGI_FORMAT_R32G32B32A32_FLOAT,		// R32G32B32A32_Float
			DXGI_FORMAT_R32G32B32A32_UINT,		// R32G32B32A32_Uint
			DXGI_FORMAT_R32G32B32A32_SINT,		// R32G32B32A32_Sint
			DXGI_FORMAT_R32G32B32_FLOAT,		// R32G32B32_Float
			DXGI_FORMAT_R32G32B32_UINT,			// R32G32B32_Uint
			DXGI_FORMAT_R32G32B32_SINT,			// R32G32B32_Sint
			DXGI_FORMAT_R32G32_FLOAT,			// R32G32_Float
			DXGI_FORMAT_R32G32_UINT,			// R32G32_Uint
			DXGI_FORMAT_R32G32_SINT,			// R32G32_Sint
			DXGI_FORMAT_R32_FLOAT,				// R32_Float
			DXGI_FORMAT_R32_UINT,				// R32_Uint
			DXGI_FORMAT_R32_SINT,				// R32_Sint
			DXGI_FORMAT_R16G16B16A16_FLOAT,		// R16G16B16A16_Float
			DXGI_FORMAT_R16G16B16A16_UNORM,		// R16G16B16A16_Unorm
			DXGI_FORMAT_R16G16B16A16_UINT,		// R16G16B16A16_Uint
			DXGI_FORMAT_R16G16B16A16_SNORM,		// R16G16B16A16_Snorm
			DXGI_FORMAT_R16G16B16A16_SINT,		// R16G16B16A16_Sint
			DXGI_FORMAT_R16G16_FLOAT,			// R16G16_Float
			DXGI_FORMAT_R16G16_UNORM,			// R16G16_Unorm
			DXGI_FORMAT_R16G16_UINT,			// R16G16_Uint
			DXGI_FORMAT_R16G16_SNORM,			// R16G16_Snorm
			DXGI_FORMAT_R16G16_SINT,			// R16G16_Sint
			DXGI_FORMAT_R16_FLOAT,				// R16_Float
			DXGI_FORMAT_R16_UNORM,				// R16_Unorm
			DXGI_FORMAT_R16_UINT,				// R16_Uint
			DXGI_FORMAT_R16_SNORM,				// R16_Snorm
			DXGI_FORMAT_R16_SINT,				// R16_Sint
			DXGI_FORMAT_R8G8B8A8_UNORM,			// R8G8B8A8_Unorm
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,	// R8G8B8A8_Unorm_Srgb
			DXGI_FORMAT_R8G8B8A8_UINT,			// R8G8B8A8_Uint
			DXGI_FORMAT_R8G8B8A8_SNORM,			// R8G8B8A8_Snorm
			DXGI_FORMAT_R8G8B8A8_SINT,			// R8G8B8A8_Sint
			DXGI_FORMAT_R8G8_UNORM,				// R8G8_Unorm
			DXGI_FORMAT_R8G8_UINT,				// R8G8_Uint
			DXGI_FORMAT_R8G8_SNORM,				// R8G8_Snorm
			DXGI_FORMAT_R8G8_SINT,				// R8G8_Sint
			DXGI_FORMAT_R8_UNORM,				// R8_Unorm
			DXGI_FORMAT_R8_UINT,				// R8_Uint
			DXGI_FORMAT_R8_SNORM,				// R8_Snorm
			DXGI_FORMAT_R8_SINT,				// R8_Sint
			DXGI_FORMAT_B8G8R8A8_UNORM,			// B8G8R8A8_Unorm
			DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,	// B8G8R8A8_Unorm_Srgb
			DXGI_FORMAT_B8G8R8X8_UNORM,			// B8G8R8X8_Unorm
			DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,	// B8G8R8X8_Unorm_Srgb
			DXGI_FORMAT_R10G10B10A2_UNORM,		// R10G10B10A2_Unorm
			DXGI_FORMAT_R10G10B10A2_UINT,		// R10G10B10A2_Uint
			DXGI_FORMAT_R11G11B10_FLOAT,		// R11G11B10_Float
			DXGI_FORMAT_D32_FLOAT,				// D32_Float
			DXGI_FORMAT_D24_UNORM_S8_UINT,		// D24_Unorm_S8_Uint
			DXGI_FORMAT_D16_UNORM,				// D16_Unorm
			DXGI_FORMAT_BC1_UNORM,				// BC1_Unorm
			DXGI_FORMAT_BC1_UNORM_SRGB,			// BC1_Unorm_Srgb
			DXGI_FORMAT_BC2_UNORM,				// BC2_Unorm
			DXGI_FORMAT_BC2_UNORM_SRGB,			// BC2_Unorm_Srgb
			DXGI_FORMAT_BC3_UNORM,				// BC3_Unorm
			DXGI_FORMAT_BC3_UNORM_SRGB,			// BC3_Unorm_Srgb
			DXGI_FORMAT_BC4_UNORM,				// BC4_Unorm
			DXGI_FORMAT_BC4_SNORM,				// BC4_Snorm
			DXGI_FORMAT_BC5_UNORM,				// BC5_Unorm
			DXGI_FORMAT_BC5_SNORM,				// BC5_Snorm
			DXGI_FORMAT_BC6H_UF16,				// BC6H_UFloat
			DXGI_FORMAT_BC6H_SF16,				// BC6H_SFloat
			DXGI_FORMAT_BC7_UNORM,				// BC7_Unorm
			DXGI_FORMAT_BC7_UNORM_SRGB,			// BC7_Unorm_Srgb
		};
		return k[v];
	}

	/**
	 * @brief get d3d12 resource dimension.
	*/
	inline D3D12_RESOURCE_DIMENSION GetNativeResourceDimension(ResourceDimension::Type v)
	{
		static const D3D12_RESOURCE_DIMENSION k[] = {
			D3D12_RESOURCE_DIMENSION_BUFFER,
			D3D12_RESOURCE_DIMENSION_TEXTURE1D,
			D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			D3D12_RESOURCE_DIMENSION_TEXTURE3D
		};
		return k[v];
	}

	/**
	 * @brief get d3d12 resource state.
	*/
	inline D3D12_RESOURCE_STATES GetNativeResourceState(ResourceState::Type v)
	{
		static const D3D12_RESOURCE_STATES k[] = {
			D3D12_RESOURCE_STATE_COMMON,								// Unknown
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,			// ConstantBuffer
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,			// VertexBuffer
			D3D12_RESOURCE_STATE_INDEX_BUFFER,							// IndexBuffer
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// ShaderResource
			D3D12_RESOURCE_STATE_RENDER_TARGET,							// RenderTarget
			D3D12_RESOURCE_STATE_DEPTH_WRITE,							// DepthWrite
			D3D12_RESOURCE_STATE_DEPTH_READ,							// DepthRead
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,						// UnorderedAccess
			D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT,						// IndirectArg
			D3D12_RESOURCE_STATE_COPY_SOURCE,							// CopySrc
			D3D12_RESOURCE_STATE_COPY_DEST,								// CopyDst
			D3D12_RESOURCE_STATE_PRESENT,								// Present
			D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,		// AccelerationStructure
			D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE,					// ShadingRate
		};
		return k[v];
	}

}
//	EOF
