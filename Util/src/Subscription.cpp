#include "Arc_Assert.hpp"
#include "Subscription.hpp"

namespace Util
{
	Subscription::Subscription(UnsubscribeCallback unsubscribeCallback)
		: m_unsubscribeCallback(unsubscribeCallback)
	{}

	Subscription::Subscription(Subscription&& other)
		: m_unsubscribeCallback(other.m_unsubscribeCallback)
		, m_isSubscribed(other.m_isSubscribed)
	{
		other.m_isSubscribed = false;
		other.m_unsubscribeCallback = [](){}; // just assign to an empty function
	}

	Subscription& Subscription::operator=(Subscription&& other)
	{
		if (this != &other)
		{
			// remove the existing subscription
			Unsubscribe();

			m_isSubscribed = other.m_isSubscribed;
			m_unsubscribeCallback = other.m_unsubscribeCallback;

			other.m_isSubscribed = false;
			other.m_unsubscribeCallback = [](){}; // just assign to an empty function
		}

		return *this;
	}

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