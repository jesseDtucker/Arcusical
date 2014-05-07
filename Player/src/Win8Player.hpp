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
	class MediaEngineNotify : public IMFMediaEngineNotify
	{
	public:
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
	};

	class Win8Player final : public IPlayer
	{
	public:
		Win8Player();
		virtual ~Win8Player();

		virtual void SetSong(std::shared_ptr<Model::Song> song) override;
		virtual std::shared_ptr<Model::Song> GetCurrentSong() override;

		virtual void Play() override;
		virtual void Stop() override;

	private:
		void PlayNativeSong(Model::SongStream& stream);

		std::shared_ptr<Model::Song> m_currentSong;
		MediaEngineNotify m_mediaEngineNotify;
		bool m_isCurrentSongSetForPlay;

		CComPtr<IMFMediaEngineClassFactory> m_factory;
		CComPtr<IMFAttributes> m_attributes;
		CComPtr<IMFMediaEngineEx> m_mediaEngine;
		
	};
}
}

#endif