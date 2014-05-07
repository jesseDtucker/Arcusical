#pragma once
#ifndef EVENT_SERVICE_HPP

#include <memory>
#include <set>

#include "Delegate.hpp"
#include "ServiceResolver.hpp"
#include "Subscription.hpp"

namespace Arcusical
{
namespace Events
{
	template<typename Event>
	class EventService
	{
	public:
		static std::shared_ptr<Util::Subscription> RegisterListener(Util::Delegate<void(const Event&)> callback);
		static void BroadcastEvent(const Event&);

	private:
		static std::set<Util::Delegate<void(const Event&)>> s_callbackSet;
	};

	template<typename Event>
	std::set<Util::Delegate<void(const Event&)>> EventService<Event>::s_callbackSet;

	template<typename Event>
	std::shared_ptr<Util::Subscription> EventService<Event>::RegisterListener(Util::Delegate<void(const Event&)> callback)
	{
		std::function<void()> unsubscribeCallback = [callback](){ s_callbackSet.erase(callback); };
		auto subscription = std::make_shared<Util::Subscription>(unsubscribeCallback);

		s_callbackSet.insert(callback);

		return subscription;
	}

	template<typename Event>
	void EventService<Event>::BroadcastEvent(const Event& event)
	{
		for (const auto& callback : s_callbackSet)
		{
			callback(event);
		}
	}
}
}

#endif