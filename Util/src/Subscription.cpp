#include "Arc_Assert.hpp"
#include "Subscription.hpp"

namespace Util
{
	Subscription::Subscription(UnsubscribeCallback unsubscribeCallback)
		: m_unsubscribeCallback(unsubscribeCallback)
		, m_isSubscribed(true)
	{}

	Subscription::Subscription(Subscription&& other)
		: m_unsubscribeCallback(other.m_unsubscribeCallback)
		, m_isSubscribed(other.m_isSubscribed)
	{
		other.m_isSubscribed = false;
		other.m_unsubscribeCallback = nullptr;
	}

	Subscription::Subscription()
		: m_unsubscribeCallback(nullptr)
		, m_isSubscribed(false)
	{

	}

	Subscription::Subscription(std::nullptr_t)
		: Subscription()
	{

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
			other.m_unsubscribeCallback = nullptr;
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
			if (m_unsubscribeCallback != nullptr)
			{
				m_unsubscribeCallback();
			}
			m_isSubscribed = false;
		}
	}
}