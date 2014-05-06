#ifndef SONG_VM_HPP
#define SONG_VM_HPP

#include <memory>

#include "Commands/PlaySongCommand.hpp"
#include "Stream.hpp"
#include "PropertyHelper.hpp"
#include "Utility/XamlMacros.hpp"

namespace Arcusical {
namespace Commands
{
	ref class CommandWrapper;
}
namespace Model 
{
	class Song;
}
namespace Player
{
	class IPlayer;
}
}

namespace Arcusical{ namespace ViewModel {

	// This enum is expected to match the enum in the song model
	public enum class AudioFormat
	{
		UNKNOWN,
		AAC,
		ALAC,
		FLAC,
		MP3
	};

	public ref struct SongStreamVM sealed
	{
	public:
		property AudioFormat Encoding;
		property unsigned int BitRate;
		property unsigned int SampleSize;
		property unsigned int ChannelCount;
	internal:
		std::shared_ptr<Util::Stream> Stream;
	};

	[Windows::UI::Xaml::Data::Bindable]
	public ref class SongVM sealed : INotifyPropertyChanged_t
	{
	public:
		NOTIFY_PROPERTY_CHANGED_IMPL;

		PROP_SET_AND_GET_WINRT(Platform::String^, Title);
		PROP_SET_AND_GET_WINRT(Platform::String^, Artist);
		PROP_SET_AND_GET_WINRT(unsigned long long, Length);
		PROP_SET_AND_GET_WINRT(AudioFormat, Format);
		PROP_SET_AND_GET_WINRT(Commands::CommandWrapper^, PlayPauseSong);
		
		property Platform::Boolean IsPlaying
		{
			Platform::Boolean get();
		}

		SongStreamVM^ GetMediaStream();

		static SongVM^ GetEmptySong();

	internal:
		SongVM(std::shared_ptr<Model::Song> song);
		std::shared_ptr<Model::Song> GetModel();

		Commands::PlaySongCommand& PlaySong();
		
	private:
		SongVM();
		void Play();

		std::shared_ptr<Model::Song> m_song;
		Commands::PlaySongCommand m_playSongCommand;
	};

} /* ViewModel */
} /* Arcusical */

#endif