#pragma once
#ifndef SCOPE_GUARD_HPP

namespace Util
{
	template<typename CallbackType>
	class ScopeGuard final
	{
	public:
		ScopeGuard(CallbackType callback);
		~ScopeGuard();
	private:
		CallbackType m_cleanupCall;
	};

	template<typename CallbackType>
	ScopeGuard<CallbackType>::ScopeGuard(CallbackType callback)
		: m_cleanupCall(callback)
	{}

	template<typename CallbackType>
	ScopeGuard<CallbackType>::~ScopeGuard()
	{
		m_cleanupCall();
	}
}

#endif