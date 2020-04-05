#include "../include/mll/mll_defines.h"
#include "../include/mll/mll_interfaces.h"

#include <cassert>


namespace mll
{
	namespace
	{
		static const u8		kPendingKillCount = 3;
	}

	//! @brief object handle id.
	std::atomic<u64>	IDevice::objectId_;

	//-----------------------------------------------------------
	// Kill device child.
	//-----------------------------------------------------------
	void IDevice::KillDeviceChild(IDeviceChild* obj)
	{
#if _DEBUG
		auto it = liveObjects_.find(obj);
		assert(it != liveObjects_.end());
#endif
		std::lock_guard<std::mutex> lock(objectMutex_);
		obj->pendingKillCount_ = kPendingKillCount;
		liveObjects_.erase(obj);
		deathList_.push_back(obj);
	}

	//-----------------------------------------------------------
	// Iterate death list.
	//-----------------------------------------------------------
	void IDevice::ProcDeathList(bool bForce)
	{
		std::lock_guard<std::mutex> lock(objectMutex_);
		if (!bForce)
		{
			auto it = deathList_.begin();
			while (it != deathList_.end())
			{
				(*it)->pendingKillCount_--;
				if ((*it)->pendingKillCount_ == 0)
				{
					auto obj = (*it);
					it = deathList_.erase(it);
					delete obj;
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			for (auto&& obj : deathList_)
			{
				delete obj;
			}
			deathList_.clear();
		}
	}

}	// namespace mll


//	EOF
