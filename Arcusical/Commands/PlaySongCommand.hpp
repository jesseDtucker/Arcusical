#pragma once
#ifndef PLAY_SONG_COMMAND_HPP
#define PLAY_SONG_COMMAND_HPP

#include <functional>

#include "Commands/CommandBase.hpp"

namespace Arcusical{
	namespace ViewModel
	{
		ref class SongVM;
	}

	namespace Commands
	{
		class PlaySongCommand : public Commands::CommandBase<ViewModel::SongVM^>
		{
		public:
			PlaySongCommand(std::function<void(ViewModel::SongVM^)> executeCallback,
							std::function<bool(ViewModel::SongVM^)> canExecuteCallback = CommandBase<ViewModel::SongVM^>::s_emptyCanExecute);

			virtual void Execute(ViewModel::SongVM^ parameter = nullptr) override;
			virtual bool CanExecute(ViewModel::SongVM^ parameter = nullptr) override;
		};
	}
}

#endif