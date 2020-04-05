#pragma once

#include <memory>
#include <atomic>
#include <list>
#include <set>
#include <string>
#include <mutex>


namespace mll
{
	//-----------------------------------------------------------
	// forward declaration.
	//-----------------------------------------------------------
	class IDeviceChild;
	class ICommandList;
	class ISwapchain;
	class ITexture;

	//-----------------------------------------------------------
	//! @brief safe release.
	//-----------------------------------------------------------
	template <typename T>
	void SafeRelease(T*& p)
	{
		if (p)
		{
			p->Release();
			p = nullptr;
		}
	}

	//-----------------------------------------------------------
	//! @brief check failed.
	//-----------------------------------------------------------
	inline bool IsFailed(Result::Type t)
	{
		return t != Result::Ok;
	}

	//-----------------------------------------------------------
	//! @brief check succeeded.
	//-----------------------------------------------------------
	inline bool IsSucceeded(Result::Type t)
	{
		return t == Result::Ok;
	}

	/*! @name fnv1a hash functions. */
	/* @{ */
	static const u32 kFnv1aPrime32 = 16777619;
	static const u32 kFnv1aSeed32 = 0x811c9dc5;
	static const u64 kFnv1aPrime64 = 1099511628211L;
	static const u64 kFnv1aSeed64 = 0xcbf29ce484222325L;

	inline u32 CalcFnv1a32(u8 oneByte, u32 hash = kFnv1aSeed32)
	{
		return (oneByte ^ hash) * kFnv1aPrime32;
	}
	inline u32 CalcFnv1a32(const void* data, size_t numBytes, u32 hash = kFnv1aSeed32)
	{
		const u8* ptr = reinterpret_cast<const u8*>(data);
		while (numBytes--)
			hash = CalcFnv1a32(*ptr++, hash);
		return hash;
	}
	inline u64 CalcFnv1a64(u8 oneByte, u64 hash = kFnv1aSeed64)
	{
		return (oneByte ^ hash) * kFnv1aPrime32;
	}
	inline u64 CalcFnv1a64(const void* data, size_t numBytes, u64 hash = kFnv1aSeed64)
	{
		const u8* ptr = reinterpret_cast<const u8*>(data);
		while (numBytes--)
			hash = CalcFnv1a64(*ptr++, hash);
		return hash;
	}
	/* @} */


	//-----------------------------------------------------------
	//! @brief Safe object pointer.
	//-----------------------------------------------------------
	template <typename T>
	class ObjPtr
	{
		template <typename T>
		friend class ObjWeakPtr;

		friend class IDevice;

	public:
		typedef typename ObjPtr<T> SelfType;

		ObjPtr()
		{}
		ObjPtr(const SelfType& t)
			: obj_(t.obj_)
			, id_(t.id_)
		{}

		void Reset()
		{
			obj_.reset();
		}

		bool IsValid() const
		{
			return (obj_.get() != nullptr);
		}

		operator const T* () const
		{
			return obj_.get();
		}
		operator T* ()
		{
			return obj_.get();
		}
		const T* operator->() const
		{
			return obj_.get();
		}
		T* operator->()
		{
			return obj_.get();
		}

		bool operator==(const SelfType& t) const
		{
			return id_ == t.id_;
		}
		bool operator<(const SelfType& t) const
		{
			return id_ < t.id_;
		}
		bool operator>(const SelfType& t) const
		{
			return id_ > t.id_;
		}

	private:
		ObjPtr(T* obj, u64 id)
			: obj_(obj, [](T* p) { p->Release(); })
			, id_(id)
		{}

		std::shared_ptr<T>	obj_;
		u64					id_ = 0;
	};	// class ObjPtr

	//-----------------------------------------------------------
	//! @brief Safe object weak pointer.
	//-----------------------------------------------------------
	template <typename T>
	class ObjWeakPtr
	{
	public:
		typedef typename ObjWeakPtr<T> SelfType;

		ObjWeakPtr()
		{}
		ObjWeakPtr(const SelfType& t)
			: obj_(t.obj_)
			, id_(t.id_)
		{}
		ObjWeakPtr(const ObjPtr<T>& t)
			: obj_(t.obj_)
			, id_(t.id_)
		{}

		bool IsValid() const
		{
			return !obj_.expired();
		}

		operator const T* () const
		{
			return obj_.lock().get();
		}
		operator T* ()
		{
			return obj_.lock().get();
		}
		const T* operator->() const
		{
			return obj_.lock().get();
		}
		T* operator->()
		{
			return obj_.lock().get();
		}

	private:
		std::weak_ptr<T>	obj_;
		u64					id_ = 0;
	};	// class ObjWeakPtr

	//-----------------------------------------------------------
	//! @brief Graphics device interface.
	//-----------------------------------------------------------
	class IDevice
	{
		template <typename T>
		friend class ObjPtr;
		friend class IDeviceChild;

	public:
		/**
		 * @brief Iterate death list.
		 *
		 * @param[in]	bForce		強制的に全てのオブジェクトを削除するフラグ
		*/
		void ProcDeathList(bool bForce = false);

		/**
		 * @brief Iterate live objects.
		 *
		 * @param[in]	func		オブジェクトごとに実行する関数
		 * @return					生き残っているオブジェクト数
		*/
		template <typename TFunc>
		u32 IterateLiveObjects(TFunc func)
		{
			u32 ret = 0;
			for (auto&& obj : liveObjects_)
			{
				func(obj);
				ret++;
			}
			return ret;
		}

		/**
		 * @brief Get raytracing is enabled, or not.
		*/
		bool GetEnableRaytracing() const
		{
			return enableRaytracing_;
		}

	private:
		/**
		 * @brief Kill device child.
		 *
		 * @param[in]	obj			削除するオブジェクト
		*/
		void KillDeviceChild(IDeviceChild* obj);

		// --- @start these functions implement in each platform library.
	public:
		/**
		 * @brief create command list.
		*/
		Result::Type CreateCommandList(const CommandListDesc& desc, ObjPtr<ICommandList>& outObj);

		/**
		 * @brief create swapchain.
		*/
		Result::Type CreateSwapchain(const SwapchainDesc& desc, ObjPtr<ISwapchain>& outObj);

		/**
		 * @brief create texture.
		*/
		Result::Type CreateTexture(const TextureDesc& desc, ObjPtr<ITexture>& outObj);

	private:
		/**
		 * @brief Release device.
		*/
		void Release();

		// --- @end these functions implement in each platform library.

	protected:
		IDevice()
		{}
		virtual ~IDevice()
		{}

		IDevice(const IDevice&) = delete;
		IDevice& operator=(const IDevice&) = delete;

		template <typename T>
		ObjPtr<T> AppendDeviceChild(T* obj)
		{
			std::lock_guard<std::mutex> lock(objectMutex_);
			liveObjects_.insert(obj);
			u64 id = objectId_.fetch_add(1);
			obj->SetObjectId(id);
			obj->SetParentDevice(this);
			return ObjPtr<T>(obj, id);
		}

	protected:
		std::mutex					objectMutex_;			// オブジェクト追加、削除用Mutex
		std::set<IDeviceChild*>		liveObjects_;			// 自身が生成したオブジェクト
		std::list<IDeviceChild*>	deathList_;				// デスリスト

		bool	enableRaytracing_ = false;

	private:
		static std::atomic<u64>	objectId_;

	public:
		/**
		 * @brief Graphics device create function.
		 *
		 * @note This function implement in each platform library.
		*/
		static ObjPtr<IDevice> CreateGraphicsDevice(const DeviceDesc& desc);
	};	// class IDevice

	//-----------------------------------------------------------
	//! @brief Device child interface.
	//-----------------------------------------------------------
	class IDeviceChild
	{
		friend class IDevice;

	protected:
		template <typename T>
		friend class ObjPtr;

	public:
		/**
		 * @brief Get object id.
		*/
		u64 GetObjectId() const
		{
			return objectId_;
		}

		/**
		 * @brief Get object name.
		*/
		const std::string& GetObjectName() const
		{
			return objectName_;
		}

		/**
		 * @brief Get object type.
		*/
		virtual const char* GetObjectType() const = 0;

	protected:
		IDeviceChild()
		{}
		virtual ~IDeviceChild()
		{}

		IDeviceChild(const IDeviceChild&) = delete;
		IDeviceChild& operator=(const IDeviceChild&) = delete;

		void SetObjectName(const std::string& name)
		{
			objectName_ = name;
		}

		void KillSelf()
		{
			pParentDevice_->KillDeviceChild(this);
		}

	private:
		/**
		 * @brief Release self.
		*/
		virtual void Release() = 0;

		void SetParentDevice(IDevice* p)
		{
			pParentDevice_ = p;
		}
		void SetObjectId(u64 id)
		{
			objectId_ = id;
		}

	protected:
		IDevice*		pParentDevice_ = nullptr;
	private:
		std::string		objectName_ = "";
		u64				objectId_ = 0;
		u8				pendingKillCount_ = 0;
	};	// class IDeviceChild

	//-----------------------------------------------------------
	//! @brief Graphics command list interface.
	//-----------------------------------------------------------
	class ICommandList
		: public IDeviceChild
	{
	public:
		/**
		 * @brief Get object type.
		*/
		const char* GetObjectType() const override
		{
			return "CommandList";
		}

		/**
		 * @brief Get initial desc.
		*/
		const CommandListDesc& GetDesc() const
		{
			return desc_;
		}

		// --- @start these functions implement in each platform library.
		/**
		 * @brief begin command load.
		*/
		void Begin();

		/**
		 * @brief end command load.
		*/
		void End();
		// --- @end these functions implement in each platform library.

	protected:
		ICommandList()
		{}
		virtual ~ICommandList()
		{}

		CommandListDesc		desc_;
	};	// class ICommandList

	//-----------------------------------------------------------
	//! @brief Swapchain interface.
	//-----------------------------------------------------------
	class ISwapchain
		: public IDeviceChild
	{
	public:
		/**
		 * @brief get object type.
		*/
		const char* GetObjectType() const override
		{
			return "Swapchain";
		}

		/**
		 * @brief get initial desc.
		*/
		const SwapchainDesc& GetDesc() const
		{
			return desc_;
		}

		// --- @start these functions implement in each platform library.
		/**
		 * @brief present swapchain.
		*/
		void Present(u32 syncInterval);

		/**
		 * @brief get current backbuffer index.
		*/
		u32 GetBackBufferIndex() const;
		// --- @end these functions implement in each platform library.

	protected:
		ISwapchain()
		{}
		virtual ~ISwapchain()
		{}

		SwapchainDesc	desc_;
	};	// class ISwapchain

	//-----------------------------------------------------------
	//! @brief texture interface.
	//-----------------------------------------------------------
	class ITexture
		: public IDeviceChild
	{
	public:
		/**
		 * @brief get object type.
		*/
		const char* GetObjectType() const override
		{
			return "Texture";
		}

		/**
		 * @brief get initial desc.
		*/
		const TextureDesc& GetDesc() const
		{
			return desc_;
		}

		// --- @start these functions implement in each platform library.
		// --- @end these functions implement in each platform library.

	protected:
		ITexture()
		{}
		virtual ~ITexture()
		{}

		TextureDesc	desc_;
	};	// class ITexture

}

//! @brief new delete interfaces.
//! @{
#define MLL_NEW(T, ...)		\
			new T(__VA_ARGS__)
#define MLL_NEW_ARRAY(T, n)	\
			new T[n]
#define MLL_DELETE(p)		\
			delete p
#define MLL_DELETE_ARRAY(p)	\
			delete[] p
//! @}

//	EOF
