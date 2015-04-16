#pragma once
#ifndef WIN_8_PLAYER
#define WIN_8_PLAYER

#include <atomic>
#include <atlcomcli.h>
#include <wrl/client.h>
#include <mfapi.h>
#include <Mfmediaengine.h>

#include "IPlayer.hpp"

namespace Arcusical
{
	namespace Model
	{
		struct SongStream;
	}
namespace Player
{
	class Win8Player;

	class MediaEngineNotify : public IMFMediaEngineNotify
	{
	public:

		void SetPlayer(Win8Player* player);

		// IMFMediaEngineNotify + IUnknown interface
		virtual HRESULT STDMETHODCALLTYPE EventNotify
			(
			_In_ DWORD event,
			_In_ DWORD_PTR param1,
			_In_ DWORD param2
			);
		virtual ULONG STDMETHODCALLTYPE AddRef();
		virtual ULONG STDMETHODCALLTYPE Release();
		virtual HRESULT STDMETHODCALLTYPE QueryInterface
			(
			_In_ REFIID riid,
			_Out_ LPVOID* ppvObj
			);

	private:
		std::atomic<ULONG> m_refCount = 1;
		Win8Player* m_player = nullptr;
	};

	class Win8Player final : public IPlayer
	{
	public:
		Win8Player();
		virtual ~Win8Player();

		virtual void SetSong(const Model::Song& song) override;
		virtual Model::Song* GetCurrentSong() override;

		virtual void Play() override;
		virtual void Stop() override;
		virtual void ClearSong() override;

		virtual double GetCurrentTime() const override;
		virtual double GetDuration() const override;
		virtual void SetCurrentTime(double time) override;

		virtual void SetVolume(double volume) override;
		virtual double GetVolume() const override;

	private:
		void PlayNativeSong(Model::SongStream& stream);

		boost::optional<Model::Song> m_currentSong;
		MediaEngineNotify m_mediaEngineNotify;
		bool m_isCurrentSongSetForPlay;

		CComPtr<IMFMediaEngineClassFactory> m_factory;
		CComPtr<IMFAttributes> m_attributes;
		CComPtr<IMFMediaEngineEx> m_mediaEngine;

		Windows::Media::MediaExtensionManager^ m_extensionManager;
		
		friend MediaEngineNotify;
	};
}
}

#endif
