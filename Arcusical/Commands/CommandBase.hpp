#pragma once
#ifndef COMMAND_BASE_HPP
#define COMMAND_BASE_HPP

#include <functional>

namespace Arcusical
{
namespace Commands
{
	class NativeCommand
	{
	public:
		virtual Platform::Boolean CanExecute(Platform::Object^ parameter) = 0;
		virtual void Execute(Platform::Object^ parameter) = 0;
	};

	template<class ParamT>
	class CommandBase : public NativeCommand
	{
	protected:
		static std::function<bool(ParamT)> s_emptyCanExecute;
	public:
		virtual bool CanExecute(Platform::Object^ parameter) override;
		virtual void Execute(Platform::Object^ parameter) override;

		virtual bool CanExecute(ParamT parameter);
		virtual void Execute(ParamT parameter);
		
		CommandBase(std::function<void(ParamT)> executeCallback,
			std::function<bool(ParamT)> canExecuteCallback = s_emptyCanExecute);

	private:
		std::function<bool(ParamT)> m_canExecuteCallback;
		std::function<void(ParamT)> m_executeCallback;
	};

	template<typename ParamT>
	std::function<bool(ParamT)> CommandBase<ParamT>::s_emptyCanExecute = [](ParamT){ return true; };

	template<typename ParamT>
	CommandBase<ParamT>::CommandBase(std::function<void(ParamT)> executeCallback, std::function<bool(ParamT)> canExecuteCallback)
		: m_executeCallback(executeCallback)
		, m_canExecuteCallback(canExecuteCallback)
	{}

	template<typename ParamT>
	Platform::Boolean CommandBase<ParamT>::CanExecute(Platform::Object^ parameter)
	{
		auto param = (ParamT) (parameter);
		ARC_ASSERT(param != nullptr);
		if (param != nullptr)
		{
			return CanExecute(param);
		}
		else
		{
			return false;
		}
	}

	template<typename ParamT>
	Platform::Boolean CommandBase<ParamT>::CanExecute(ParamT parameter)
	{
		return m_canExecuteCallback(parameter);
	}

	template<typename ParamT>
	void CommandBase<ParamT>::Execute(Platform::Object^ parameter)
	{
		auto param = (ParamT) (parameter);
		ARC_ASSERT(param != nullptr);
		if (param != nullptr)
		{
			Execute(param);
		}
	}

	template<typename ParamT>
	void CommandBase<ParamT>::Execute(ParamT parameter)
	{
		m_executeCallback(parameter);
	}
}
}

#endif