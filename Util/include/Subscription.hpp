#ifndef SUBSCRIPTION_HPP
#define SUBSCRIPTION_HPP

#include <memory>

#include "Delegate.hpp"

namespace Util
{
	class Subscription;

	typedef std::unique_ptr<Subscription> SubscriptionPtr;

	class Subscription final
	{
	public:
		typedef std::function<void()> UnsubscribeCallback;
		Subscription();
		Subscription(std::nullptr_t);
		Subscription(UnsubscribeCallback unsubscribeCallback);
		Subscription(Subscription&&);
		Subscription& operator =(Subscription&&);
		Subscription(const Subscription&) = delete; // copy is not allowed. If needs to be shared please place in shared_ptr
		Subscription& operator =(const Subscription&) = delete;
		~Subscription();

		void Unsubscribe();
	private:
		bool m_isSubscribed = true;
		UnsubscribeCallback m_unsubscribeCallback;
	};
}

#endif