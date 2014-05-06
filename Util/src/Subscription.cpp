#include "pch.h"

#include "Subscription.hpp"

namespace Util
{
	Subscription::Subscription(UnsubscribeCallback unsubscribeCallback)
		: m_unsubscribeCallback(unsubscribeCallback)
	{}

	Subscription::~Subscription()
	{
		Unsubscribe();
	}

	void Subscription::Unsubscribe()
	{
		if (m_isSubscribed)
		{
			m_unsubscribeCallback();
			m_isSubscribed = false;
		}
	}
}