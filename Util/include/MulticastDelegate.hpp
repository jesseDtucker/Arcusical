#ifndef MULTICAST_DELEGATE_HPP
#define MULTICAST_DELEGATE_HPP

#include <unordered_set>

#include "Delegate.hpp"

namespace Util
{
	template<typename T>
	class MulticastDelegate
	{
	public:
		MulticastDelegate() = default;

		template<typename... Args>
		void operator()(Args&&... args);

		void operator+=(const Delegate<T>& rhs);
		Delegate<T> operator+=(const std::function<T>& rhs);
		void operator-=(const Delegate<T>& rhs);
	private:
		std::unordered_set<Delegate<T>> m_delegates;
	};

	/************************************************************************/
	/* Implementation                                                       */
	/************************************************************************/

	template<typename T>
	template<typename... Args>
	void MulticastDelegate<T>::operator()(Args&&... args)
	{
		for (auto del : m_delegates)
		{
			del(std::forward<Args>(args)...);
		}
	}

	template<typename T>
	void MulticastDelegate<T>::operator+=(const Delegate<T>& rhs)
	{
		m_delegates.insert(rhs);
	}

	template<typename T>
	Delegate<T> MulticastDelegate<T>::operator+=(const std::function<T>& rhs)
	{
		Delegate<T> result(rhs);
		(*this) += result;
		return result;
	}

	template<typename T>
	void MulticastDelegate<T>::operator-=(const Delegate<T>& rhs)
	{
		m_delegates.erase(rhs);
	}

}

#endif