#ifndef SUBSCRIPTION_HPP
#define SUBSCRIPTION_HPP

#include "Delegate.hpp"

namespace Util
{
	class Subscription final
	{
	public:
		typedef std::function<void()> UnsubscribeCallback;
		Subscription(UnsubscribeCallback unsubscribeCallback);
		~Subscription();

		void Unsubscribe();
	private:
		bool m_isSubscribed = true;
		UnsubscribeCallback m_unsubscribeCallback;
	};
}

#endif