#pragma once
#ifndef COMMAND_WRAPPER_HPP
#define COMMAND_WRAPPER_HPP

namespace Arcusical
{
namespace Commands
{
	class NativeCommand;

	public ref class CommandWrapper sealed : Windows::UI::Xaml::Input::ICommand
	{
	public:
		virtual Platform::Boolean CanExecute(Platform::Object^ parameter);
		virtual void Execute(Platform::Object^ parameter);
		virtual event Windows::Foundation::EventHandler<Platform::Object^>^ CanExecuteChanged; // TODO::JT currently not using!
	internal:
		CommandWrapper(NativeCommand& nativeCommand);
	private:
		NativeCommand& m_nativeCommand;
	};
}
}

#endif