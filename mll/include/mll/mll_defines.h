#pragma once

#include <cstdint>


#define MLL_ENUM_START(name)	struct name##{ enum Type {
#define MLL_ENUM_END			}; };
#define MLL_ENUM_END_WITH_MAX	MAX }; }

namespace mll
{
	typedef std::int8_t		s8;
	typedef std::uint8_t	u8;
	typedef std::int16_t	s16;
	typedef std::uint16_t	u16;
	typedef std::int32_t	s32;
	typedef std::uint32_t	u32;
	typedef std::int64_t	s64;
	typedef std::uint64_t	u64;
	typedef float			f32;
	typedef double			f64;

	//-----------------------------------------------------------
	//! @brief Process result.
	//-----------------------------------------------------------
	MLL_ENUM_START(Result)
		Ok,
		InvalidArgs,
		InvalidOperation,
		OutOfMemory,
	MLL_ENUM_END_WITH_MAX;

	//-----------------------------------------------------------
	//! @brief Command queue types.
	//-----------------------------------------------------------
	MLL_ENUM_START(CommandQueueType)
		Graphics,
		Compute,
		Copy,
	MLL_ENUM_END_WITH_MAX;

	//-----------------------------------------------------------
	//! @brief Resource formats.
	//-----------------------------------------------------------
	MLL_ENUM_START(ResourceFormat)
		Unknown,
		R32G32B32A32_Float,
		R32G32B32A32_Uint,
		R32G32B32A32_Sint,
		R32G32B32_Float,
		R32G32B32_Uint,
		R32G32B32_Sint,
		R32G32_Float,
		R32G32_Uint,
		R32G32_Sint,
		R32_Float,
		R32_Uint,
		R32_Sint,
		R16G16B16A16_Float,
		R16G16B16A16_Unorm,
		R16G16B16A16_Uint,
		R16G16B16A16_Snorm,
		R16G16B16A16_Sint,
		R16G16_Float,
		R16G16_Unorm,
		R16G16_Uint,
		R16G16_Snorm,
		R16G16_Sint,
		R16_Float,
		R16_Unorm,
		R16_Uint,
		R16_Snorm,
		R16_Sint,
		R8G8B8A8_Unorm,
		R8G8B8A8_Unorm_Srgb,
		R8G8B8A8_Uint,
		R8G8B8A8_Snorm,
		R8G8B8A8_Sint,
		R8G8_Unorm,
		R8G8_Uint,
		R8G8_Snorm,
		R8G8_Sint,
		R8_Unorm,
		R8_Uint,
		R8_Snorm,
		R8_Sint,
		B8G8R8A8_Unorm,
		B8G8R8A8_Unorm_Srgb,
		B8G8R8X8_Unorm,
		B8G8R8X8_Unorm_Srgb,
		R10G10B10A2_Unorm,
		R10G10B10A2_Uint,
		R11G11B10_Float,
		D32_Float,
		D24_Unorm_S8_Uint,
		D16_Unorm,
		BC1_Unorm,
		BC1_Unorm_Srgb,
		BC2_Unorm,
		BC2_Unorm_Srgb,
		BC3_Unorm,
		BC3_Unorm_Srgb,
		BC4_Unorm,
		BC4_Snorm,
		BC5_Unorm,
		BC5_Snorm,
		BC6H_UFloat,
		BC6H_SFloat,
		BC7_Unorm,
		BC7_Unorm_Srgb,
	MLL_ENUM_END_WITH_MAX;

	//-----------------------------------------------------------
	//! @brief resource heap type.
	//-----------------------------------------------------------
	MLL_ENUM_START(ResourceHeap)
		Default,
		Dynamic,
		Readback,
	MLL_ENUM_END_WITH_MAX;

	//-----------------------------------------------------------
	//! @brief resource dimension type.
	//-----------------------------------------------------------
	MLL_ENUM_START(ResourceDimension)
		Buffer,
		Texture1D,
		Texture2D,
		Texture3D,
	MLL_ENUM_END_WITH_MAX;

	//-----------------------------------------------------------
	//! @brief resource usage flag.
	//-----------------------------------------------------------
	MLL_ENUM_START(ResourceUsageFlag)
		ConstantBuffer			= 0x01 << 0,
		VertexBuffer			= 0x01 << 1,
		IndexBuffer				= 0x01 << 2,
		ShaderResource			= 0x01 << 3,
		RenderTarget			= 0x01 << 4,
		DepthStencil			= 0x01 << 5,
		UnorderedAccess			= 0x01 << 6,
		IndirectArg				= 0x01 << 7,
	MLL_ENUM_END;

	//-----------------------------------------------------------
	//! @brief resource state.
	//-----------------------------------------------------------
	MLL_ENUM_START(ResourceState)
		Unknown,
		ConstantBuffer,
		VertexBuffer,
		IndexBuffer,
		ShaderResource,
		RenderTarget,
		DepthWrite,
		DepthRead,
		UnorderedAccess,
		IndirectArg,
		CopySrc,
		CopyDst,
		Present,
		AccelerationStructure,
		ShadingRate,
	MLL_ENUM_END_WITH_MAX;


	//-----------------------------------------------------------
	//! @brief Graphics device description.
	//-----------------------------------------------------------
	struct DeviceDesc
	{
		bool		enableDebugLayer = false;

		DeviceDesc& SetEnableDebugLayer(bool b)
		{
			enableDebugLayer = b;
			return *this;
		}
	};	// struct DeviceDesc

	//-----------------------------------------------------------
	//! @brief Graphics command list description.
	//-----------------------------------------------------------
	struct CommandListDesc
	{
		CommandQueueType::Type	typeCommandQueue = CommandQueueType::Graphics;

		CommandListDesc& SetCommandQueueType(CommandQueueType::Type t)
		{
			typeCommandQueue = t;
			return *this;
		}
	};	// struct CommandListDesc

	//-----------------------------------------------------------
	//! @brief Swapchain description.
	//-----------------------------------------------------------
	struct SwapchainDesc
	{
		u32						width = 0;
		u32						height = 0;
		ResourceFormat::Type	format = ResourceFormat::Unknown;
		u32						backBufferCount = 0;
		void*					windowHandle = nullptr;
		bool					isFullscreen = false;

		SwapchainDesc& SetWidth(u32 v)
		{
			width = v;
			return *this;
		}
		SwapchainDesc& SetHeight(u32 v)
		{
			height = v;
			return *this;
		}
		SwapchainDesc& SetFormat(ResourceFormat::Type v)
		{
			format = v;
			return *this;
		}
		SwapchainDesc& SetBackBufferCount(u32 v)
		{
			backBufferCount = v;
			return *this;
		}
		SwapchainDesc& SetWindowHandle(void* v)
		{
			windowHandle = v;
			return *this;
		}
		SwapchainDesc& SetIsFullscreen(bool v)
		{
			isFullscreen = v;
			return *this;
		}
	};	// struct SwapchainDesc

	//-----------------------------------------------------------
	//! @brief texture description.
	//-----------------------------------------------------------
	struct TextureDesc
	{
		ResourceDimension::Type	dimension = ResourceDimension::Texture2D;
		u32						width = 0;
		u32						height = 0;
		u32						depth = 0;
		u32						arraySize = 0;
		u32						mipLevels = 0;
		ResourceFormat::Type	format = ResourceFormat::Unknown;
		u32						sampleCount = 1;
		ResourceHeap::Type		heap = ResourceHeap::Default;
		u32						usageFlags = 0;
		ResourceState::Type		initialState = ResourceState::Unknown;

		TextureDesc& SetDimension(ResourceDimension::Type v)
		{
			dimension = v;
			return *this;
		}
		TextureDesc& SetWidth(u32 v)
		{
			width = v;
			return *this;
		}
		TextureDesc& SetHeight(u32 v)
		{
			height = v;
			return *this;
		}
		TextureDesc& SetDepth(u32 v)
		{
			depth = v;
			return *this;
		}
		TextureDesc& SetArraySize(u32 v)
		{
			arraySize = v;
			return *this;
		}
		TextureDesc& SetMipLevels(u32 v)
		{
			mipLevels = v;
			return *this;
		}
		TextureDesc& SetFormat(ResourceFormat::Type v)
		{
			format = v;
			return *this;
		}
		TextureDesc& SetSampleCount(u32 v)
		{
			sampleCount = v;
			return *this;
		}
		TextureDesc& SetHeap(ResourceHeap::Type v)
		{
			heap = v;
			return *this;
		}
		TextureDesc& SetUsageFlags(u32 v)
		{
			usageFlags = v;
			return *this;
		}
		TextureDesc& SetInitialState(ResourceState::Type v)
		{
			initialState = v;
			return *this;
		}
	};	// struct TextureDesc

}	// namespace mll


//	EOF
