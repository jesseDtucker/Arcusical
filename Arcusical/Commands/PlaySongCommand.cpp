#include "pch.h"

#include "Commands/PlaySongCommand.hpp"

namespace Arcusical
{
	namespace Commands
	{
		PlaySongCommand::PlaySongCommand(	std::function<void(ViewModel::SongVM^)> executeCallback, 
											std::function<bool(ViewModel::SongVM^)> canExecuteCallback /* = CommandBase<ViewModel::SongVM^>::s_emptyCanExecute */)
		: CommandBase<ViewModel::SongVM^>(executeCallback, canExecuteCallback)
		{

		}

		void PlaySongCommand::Execute(ViewModel::SongVM^ parameter /* = nullptr */)
		{
			return CommandBase<ViewModel::SongVM^>::Execute(parameter);
		}

		bool PlaySongCommand::CanExecute(ViewModel::SongVM^ parameter /* = nullptr */)
		{
			return CommandBase<ViewModel::SongVM^>::CanExecute(parameter);
		}
	}
}