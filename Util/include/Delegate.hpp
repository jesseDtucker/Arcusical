#ifndef DELEGATE_HPP
#define DELEGATE_HPP

#include <functional>
#include <memory>
#include <utility>

namespace Util
{
	/************************************************************************/
	/*	Wraps the standard functor object std::function with a version that	*/
	/*	allows comparison and copying in a reference like manner. Can be	*/
	/*	used to store a list of callbacks and allow for comparison and		*/
	/*	removal of callbacks												*/
	/************************************************************************/
	template<typename T>
	class Delegate
	{
	public:
		Delegate(const std::function<T>& callback);
		Delegate(const Delegate& del) = default;
		Delegate& operator=(const Delegate& del) = default;

		template<typename... Args>
		void operator()(Args&&... args) const;

		bool operator==(const Delegate<T>& rhs) const;
		bool operator!=(const Delegate<T>& rhs) const;
		bool operator<(const Delegate<T>& rhs) const;
		bool operator>(const Delegate<T>& rhs) const;
		bool operator>=(const Delegate<T>& rhs)const;
		bool operator<=(const Delegate<T>& rhs) const;
	private:
		std::shared_ptr<std::function<T>> m_callback;

		friend std::hash<Delegate<T>>;
	};

	/************************************************************************/
	/* Implementation                                                       */
	/************************************************************************/

	template<typename T>
	template<typename... Args>
	void Delegate<T>::operator()(Args&&... args) const
	{
		(*m_callback)(std::forward<Args>(args)...);
	};

	template<typename T>
	Delegate<T>::Delegate(const std::function<T>& callback)
		: m_callback(std::make_shared<std::function<T>>(callback))
	{ }

	template<typename T>
	bool Delegate<T>::operator==(const Delegate<T>& rhs) const
	{
		return rhs.m_callback == this->m_callback;
	}

	template<typename T>
	bool Delegate<T>::operator !=(const Delegate<T>& rhs) const
	{
		return !(*this == rhs);
	}

	template<typename T>
	bool Delegate<T>::operator <(const Delegate<T>& rhs) const
	{
		return this->m_callback < rhs.m_callback;
	}

	template<typename T>
	bool Delegate<T>::operator <=(const Delegate<T>& rhs) const
	{
		return this->m_callback <= rhs.m_callback;
	}

	template<typename T>
	bool Delegate<T>::operator >(const Delegate<T>& rhs) const
	{
		return this->m_callback > rhs.m_callback;
	}

	template<typename T>
	bool Delegate<T>::operator >=(const Delegate<T>& rhs) const
	{
		return this->m_callback >= rhs.m_callback;
	}
}

namespace std
{
	template<typename T>
	struct hash<Util::Delegate<T>>
	{
		size_t operator()(const Util::Delegate<T>& del) const
		{
			return std::hash<decltype(del.m_callback)>()(del.m_callback);
		}
	};
}

#endif
