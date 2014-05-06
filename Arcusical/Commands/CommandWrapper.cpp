#include "pch.h"

#include "Commands/CommandWrapper.hpp"
#include "Commands/CommandBase.hpp"

namespace Arcusical
{
namespace Commands
{
	CommandWrapper::CommandWrapper(NativeCommand& nativeCommand)
		: m_nativeCommand(nativeCommand)
	{

	}

	Platform::Boolean CommandWrapper::CanExecute(Platform::Object^ parameter)
	{
		return m_nativeCommand.CanExecute(parameter);
	}

	void CommandWrapper::Execute(Platform::Object^ parameter)
	{
		m_nativeCommand.Execute(parameter);
	}
}
}