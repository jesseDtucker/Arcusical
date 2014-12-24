#ifndef SONG_VM_HPP
#define SONG_VM_HPP

#include <memory>

#include "Song.hpp"
#include "Stream.hpp"
#include "PropertyHelper.hpp"
#include "Utility/XamlMacros.hpp"

namespace Arcusical {
namespace Commands
{
	ref class CommandWrapper;
}
namespace Player
{
	class IPlayer;
}
}

namespace Arcusical{ namespace ViewModel {

	// TODO::JT is this needed?
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
		
		property Platform::Boolean IsPlaying
		{
			Platform::Boolean get();
		}

		SongStreamVM^ GetMediaStream();

		void Play();
		void Pause();

	internal:
		SongVM(const Model::Song& song);
		Model::Song* GetModel();

	private:
		Model::Song m_song;
	};

} /* ViewModel */
} /* Arcusical */

#endif