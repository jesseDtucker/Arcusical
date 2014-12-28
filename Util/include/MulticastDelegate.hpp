#ifndef MULTICAST_DELEGATE_HPP
#define MULTICAST_DELEGATE_HPP

#include <memory>
#include <unordered_set>

#include "Delegate.hpp"
#include "Subscription.hpp"

namespace Util
{
	template<typename T>
	class MulticastDelegate
	{
	public:
		MulticastDelegate() = default;

		template<typename... Args>
		void operator()(Args&&... args) const;

		Subscription operator+=(const Delegate<T>& rhs);
		Subscription operator+=(const std::function<T>& rhs);
		void operator-=(const Delegate<T>& rhs);
	private:
		std::unordered_set<Delegate<T>> m_delegates;
	};

	/************************************************************************/
	/* Implementation                                                       */
	/************************************************************************/

	template<typename T>
	template<typename... Args>
	void MulticastDelegate<T>::operator()(Args&&... args) const
	{
		for (auto del : m_delegates)
		{
			del(std::forward<Args>(args)...);
		}
	}

	template<typename T>
	Subscription MulticastDelegate<T>::operator+=(const Delegate<T>& rhs)
	{
		Subscription sub{ [rhs, this](){ (*this) -= rhs; } };
		m_delegates.insert(rhs);
		return sub;
	}

	template<typename T>
	Subscription MulticastDelegate<T>::operator+=(const std::function<T>& rhs)
	{
		Delegate<T> del(rhs);
		return ((*this) += del);
	}

	template<typename T>
	void MulticastDelegate<T>::operator-=(const Delegate<T>& rhs)
	{
		m_delegates.erase(rhs);
	}

}

#endif