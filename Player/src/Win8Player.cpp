#include <AudioSessionTypes.h>
#include <cmath>
#include <future>
#include <ppltasks.h>

#include "Arc_Assert.hpp"
#include "Song.hpp"
#include "Win8Player.hpp"

using namespace ATL;
using namespace Windows::Media;
using namespace Microsoft::WRL;
using namespace concurrency;

const std::string Arcusical::Player::IPlayer::ServiceName("Win8Player");
const double DEFAULT_VOLUME = 0.5;

namespace Arcusical
{
namespace Player
{

	Win8Player::Win8Player()
		: m_currentSong(boost::none)
		, m_mediaEngine(nullptr)
		, m_factory(nullptr)
		, m_attributes(nullptr)
		, m_isCurrentSongSetForPlay(false)
		, m_mediaEngineNotify()
	{
		ComPtr<IMFMediaEngine> engine;

		auto result = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		ARC_ThrowIfFailed(result);
		result = MFStartup(MF_VERSION);
		ARC_ThrowIfFailed(result);

		result = m_factory.CoCreateInstance(CLSID_MFMediaEngineClassFactory);
		ARC_ThrowIfFailed(result);
		
		result = MFCreateAttributes(&m_attributes, 1);
		ARC_ThrowIfFailed(result);

		result = m_attributes->SetUINT32(MF_MEDIA_ENGINE_AUDIO_CATEGORY, AudioCategory_BackgroundCapableMedia);
		ARC_ThrowIfFailed(result);

		result = m_attributes->SetUnknown(MF_MEDIA_ENGINE_CALLBACK, &m_mediaEngineNotify);
		ARC_ThrowIfFailed(result);

		result = m_factory->CreateInstance(MF_MEDIA_ENGINE_AUDIOONLY, m_attributes, &engine);
		ARC_ThrowIfFailed(result);

		result = engine.Get()->QueryInterface(__uuidof(IMFMediaEngine), (void**)(&m_mediaEngine));
		ARC_ThrowIfFailed(result);

		SetVolume(DEFAULT_VOLUME);

		auto alacInputId = MFMPEG4Format_Base;
		alacInputId.Data1 = 'alac';

		auto alacOutputId = MFAudioFormat_PCM;

		m_extensionManager = ref new MediaExtensionManager();
		m_extensionManager->RegisterAudioDecoder("ALACDecoder.ALACDecoder", alacInputId, alacOutputId);

		m_mediaEngineNotify.SetPlayer(this);
	}

	Win8Player::~Win8Player()
	{
		auto result = MFShutdown();
		ARC_ASSERT(SUCCEEDED(result));
		CoUninitialize();
	}

	void Win8Player::Play()
	{
		if (m_currentSong && m_currentSong->HasStream())
		{
			auto stream = m_currentSong->GetStream();
			PlayNativeSong(stream);
		}
	}

	void Win8Player::PlayNativeSong(Model::SongStream& stream)
	{
		HRESULT result = S_OK;

		if (!m_isCurrentSongSetForPlay)
		{
			auto file = create_task(Windows::Storage::StorageFile::GetFileFromPathAsync(ref new Platform::String(stream.songData.filePath.c_str()))).get();
			auto nativeStream = create_task(file->OpenReadAsync()).get();

			// this is kinda nasty...
			ComPtr<IUnknown> pStreamUnk = reinterpret_cast<IUnknown*>(nativeStream);
			ComPtr<IMFByteStream> pMFStream;

			result = ::MFCreateMFByteStreamOnStreamEx(pStreamUnk.Get(), &pMFStream);
			ARC_ThrowIfFailed(result);

			CComBSTR path = stream.songData.filePath.c_str();
			result = m_mediaEngine->SetSourceFromByteStream(pMFStream.Get(), path);
			ARC_ThrowIfFailed(result);
		}

		result = m_mediaEngine->Play();
		ARC_ThrowIfFailed(result);
		m_isCurrentSongSetForPlay = true;
	}

	void Win8Player::Stop()
	{
		auto result = m_mediaEngine->Pause();
		ARC_ThrowIfFailed(result);
	}

	void Win8Player::SetSong(const Model::Song& song)
	{
		if (!m_currentSong || m_currentSong != song)
		{
			m_currentSong = song;
			m_isCurrentSongSetForPlay = false;
			Stop();
			m_songChanged(m_currentSong);
		}
	}

	Model::Song* Win8Player::GetCurrentSong()
	{
		return &*m_currentSong;
	}

	double Win8Player::GetCurrentTime() const
	{
		return m_mediaEngine->GetCurrentTime();
	}

	double Win8Player::GetDuration() const
	{
		return m_mediaEngine->GetDuration();
	}

	void Win8Player::SetCurrentTime(double time)
	{
		if (!std::isnan(GetDuration()))
		{
			ARC_ASSERT(time <= GetDuration());
			time = time < GetDuration() ? time : GetDuration();
			m_mediaEngine->SetCurrentTime(time);
		}
	}

	void Win8Player::SetVolume(double volume)
	{
		auto hr = m_mediaEngine->SetVolume(volume);
		ARC_ThrowIfFailed(hr);
	}

	double Win8Player::GetVolume() const
	{
		return m_mediaEngine->GetVolume();
	}

	void Win8Player::ClearSong()
	{
		m_currentSong = boost::none;
		m_songChanged(m_currentSong);
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//	Media Engine Notify
	// 
	//////////////////////////////////////////////////////////////////////////

	HRESULT MediaEngineNotify::EventNotify(_In_ DWORD event, _In_ DWORD_PTR param1, _In_ DWORD param2)
	{
		std::async([this, event, param1, param2]()
		{
			if (m_player != nullptr)
			{
				switch (event)
				{
				case MF_MEDIA_ENGINE_EVENT_PLAYING:
					m_player->m_IsPlaying = true;
					m_player->m_IsPaused = false;
					m_player->GetPlaying()(true);
					break;
				case MF_MEDIA_ENGINE_EVENT_PAUSE:
					m_player->m_IsPlaying = false;
					m_player->m_IsPaused = true;
					m_player->GetPlaying()(false);
					break;
				case MF_MEDIA_ENGINE_EVENT_ENDED:
					m_player->m_IsPlaying = false;
					m_player->m_IsPaused = false;
					m_player->GetPlaying()(false);
					m_player->GetEnded()();
					break;
				case MF_MEDIA_ENGINE_EVENT_TIMEUPDATE:
					m_player->GetTimeUpdate()(m_player->GetCurrentTime());
					break;
				case MF_MEDIA_ENGINE_EVENT_DURATIONCHANGE:
					m_player->GetDurationChanged()(m_player->GetDuration());
					break;
				case MF_MEDIA_ENGINE_EVENT_ERROR:
					HandleError(param1, param2);
					break;
				}
			}
		});

		return S_OK;
	}

	void MediaEngineNotify::SetPlayer(Win8Player* player)
	{
		ARC_ASSERT(player != nullptr);
		m_player = player;
	}

	ULONG MediaEngineNotify::AddRef()
	{
		m_refCount++;
		return m_refCount;
	}

	ULONG MediaEngineNotify::Release()
	{
		m_refCount--;
		if (m_refCount <= 0)
		{
			// I hate this... but it's how the tutorial says to implement com objects...
			delete this;
			return 0;
		}
		return m_refCount;
	}

	HRESULT MediaEngineNotify::QueryInterface(_In_ REFIID riid, _Out_ LPVOID* ppvObj)
	{
		// Always set out parameter to NULL, validating it first.
		if (NULL == ppvObj)
			return E_INVALIDARG;
		*ppvObj = NULL;

		// not sure what the riid is for IMFMediaEngineNotify is so I'm not checking it atm
		if (riid == IID_IUnknown || riid == IID_IMFMediaEngineNotify)
		{
			// Increment the reference count and return the pointer.
			*ppvObj = (LPVOID)this;
			AddRef();
			return NOERROR;
		}
		return E_NOINTERFACE;
	}

	void MediaEngineNotify::HandleError(DWORD_PTR param1, DWORD param2)
	{
		switch (param1)
		{
		case MF_MEDIA_ENGINE_ERR_DECODE:
			HandleDecodeError(param2);
			break;
		default:
			ARC_FAIL("Unhandled media error!");
			break;
		}
	}

	void MediaEngineNotify::HandleDecodeError(DWORD param2)
	{
		// Note: cannot use switch because HRESULT_FROM_WIN32 is not constant
		if (param2 == HRESULT_FROM_WIN32(ERROR_OPLOCK_HANDLE_CLOSED))
		{
			m_player->ClearSong();
		}
		else
		{
			ARC_FAIL("Unhandled HResult");
		}
	}

}
}