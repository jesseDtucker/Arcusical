#include "pch.h"

#include <AudioSessionTypes.h>
#include <ppltasks.h>

#include "Song.hpp"
#include "Win8Player.hpp"

using namespace ATL;
using namespace Windows::Media;
using namespace Microsoft::WRL;
using namespace concurrency;

const std::string Arcusical::Player::IPlayer::ServiceName("Win8Player");

namespace Arcusical
{
namespace Player
{

	Win8Player::Win8Player()
		: m_currentSong()
		, m_mediaEngine(nullptr)
		, m_factory(nullptr)
		, m_attributes(nullptr)
		, m_isCurrentSongSetForPlay(false)
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

		auto alacInputId = MFMPEG4Format_Base;
		alacInputId.Data1 = 'alac';

		auto alacOutputId = MFAudioFormat_PCM;

		m_extensionManager = ref new MediaExtensionManager();
		m_extensionManager->RegisterAudioDecoder("ALACDecoder.ALACDecoder", alacInputId, alacOutputId);
	}

	Win8Player::~Win8Player()
	{
		auto result = MFShutdown();
		ARC_ASSERT(SUCCEEDED(result));
		CoUninitialize();
	}

	void Win8Player::Play()
	{
		auto stream = m_currentSong.GetStream();
		PlayNativeSong(stream);
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

		m_IsPlaying = true;
		m_isCurrentSongSetForPlay = true;
	}

	void Win8Player::Stop()
	{
		auto result = m_mediaEngine->Pause();
		ARC_ThrowIfFailed(result);

		m_IsPlaying = false;
	}

	void Win8Player::SetSong(const Model::Song& song)
	{
		if (m_currentSong != song)
		{
			m_currentSong = song;
			m_isCurrentSongSetForPlay = false;
			Stop();
		}
	}

	Model::Song* Win8Player::GetCurrentSong()
	{
		return &m_currentSong;
	}

	HRESULT MediaEngineNotify::EventNotify(_In_ DWORD event, _In_ DWORD_PTR param1, _In_ DWORD param2)
	{
		// TODO::JT
		switch (event)
		{
		case MF_MEDIA_ENGINE_EVENT_PLAYING :
			break;
		case MF_MEDIA_ENGINE_EVENT_ENDED:
			break;
		}
		ARC_ASSERT(event != MF_MEDIA_ENGINE_EVENT_ERROR);

		return S_OK;
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
}
}