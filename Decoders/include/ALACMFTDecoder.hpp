#pragma once

#include <Mfidl.h>
#include <mftransform.h>
#include <windows.media.h>
#include <wrl\client.h>
#include <wrl\implements.h>

#include "ALACDecoder.h"
#include "Alac.hpp"
#include "AsyncProcessor.hpp"
#include "Export.hpp"
#include "Stsd.hpp"

struct MediaInfo {
  UINT32 avgBytesPerSec = 0;
  UINT32 bitRate = 0;
  UINT32 cookieBlobSize = 0;
  UINT32 channelCount = 0;
  UINT32 samplesPerSecond = 0;
  UINT32 samplesPerFrame = 0;
  UINT32 bitsPerSample = 0;
  UINT8 *cookieBlob = nullptr;
  std::shared_ptr<Arcusical::MPEG4::Alac> alacBox;
};

class EXPORT ALACMFTDecoder WrlSealed
    : public Microsoft::WRL::RuntimeClass<
          Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>,
          ABI::Windows::Media::IMediaExtension, IMFTransform, IMFMediaEventGenerator, IMFShutdown> {
  InspectableClass(L"ALACDecoder.ALACDecoder", BaseTrust)

      public : ALACMFTDecoder();
  virtual ~ALACMFTDecoder();

  // IMediaExtension
  IFACEMETHOD(SetProperties)(ABI::Windows::Foundation::Collections::IPropertySet *pConfiguration);

  // IMFTransform methods
  STDMETHODIMP GetStreamLimits(DWORD *pdwInputMinimum, DWORD *pdwInputMaximum, DWORD *pdwOutputMinimum,
                               DWORD *pdwOutputMaximum);

  STDMETHODIMP GetStreamCount(DWORD *pcInputStreams, DWORD *pcOutputStreams);

  STDMETHODIMP GetStreamIDs(DWORD dwInputIDArraySize, DWORD *pdwInputIDs, DWORD dwOutputIDArraySize,
                            DWORD *pdwOutputIDs);

  STDMETHODIMP GetInputStreamInfo(DWORD dwInputStreamID, MFT_INPUT_STREAM_INFO *pStreamInfo);

  STDMETHODIMP GetOutputStreamInfo(DWORD dwOutputStreamID, MFT_OUTPUT_STREAM_INFO *pStreamInfo);

  STDMETHODIMP GetAttributes(IMFAttributes **pAttributes);

  STDMETHODIMP GetInputStreamAttributes(DWORD dwInputStreamID, IMFAttributes **ppAttributes);

  STDMETHODIMP GetOutputStreamAttributes(DWORD dwOutputStreamID, IMFAttributes **ppAttributes);

  STDMETHODIMP DeleteInputStream(DWORD dwStreamID);

  STDMETHODIMP AddInputStreams(DWORD cStreams, DWORD *adwStreamIDs);

  STDMETHODIMP GetInputAvailableType(DWORD dwInputStreamID, DWORD dwTypeIndex,  // 0-based
                                     IMFMediaType **ppType);

  STDMETHODIMP GetOutputAvailableType(DWORD dwOutputStreamID, DWORD dwTypeIndex,  // 0-based
                                      IMFMediaType **ppType);

  STDMETHODIMP SetInputType(DWORD dwInputStreamID, IMFMediaType *pType, DWORD dwFlags);

  STDMETHODIMP SetOutputType(DWORD dwOutputStreamID, IMFMediaType *pType, DWORD dwFlags);

  STDMETHODIMP GetInputCurrentType(DWORD dwInputStreamID, IMFMediaType **ppType);

  STDMETHODIMP GetOutputCurrentType(DWORD dwOutputStreamID, IMFMediaType **ppType);

  STDMETHODIMP GetInputStatus(DWORD dwInputStreamID, DWORD *pdwFlags);

  STDMETHODIMP GetOutputStatus(DWORD *pdwFlags);

  STDMETHODIMP SetOutputBounds(LONGLONG hnsLowerBound, LONGLONG hnsUpperBound);

  STDMETHODIMP ProcessEvent(DWORD dwInputStreamID, IMFMediaEvent *pEvent);

  STDMETHODIMP ProcessMessage(MFT_MESSAGE_TYPE eMessage, ULONG_PTR ulParam);

  STDMETHODIMP ProcessInput(DWORD dwInputStreamID, IMFSample *pSample, DWORD dwFlags);

  STDMETHODIMP ProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount,
                             MFT_OUTPUT_DATA_BUFFER *pOutputSamples,  // one per stream
                             DWORD *pdwStatus);

  // IMFMediaEventGenerator
  STDMETHODIMP BeginGetEvent(IMFAsyncCallback *pCallback, IUnknown *punkState);

  STDMETHODIMP EndGetEvent(IMFAsyncResult *pResult, IMFMediaEvent **ppEvent);

  STDMETHODIMP GetEvent(DWORD dwFlags, IMFMediaEvent **ppEvent);

  STDMETHODIMP QueueEvent(MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT *pvValue);

  // IMFShutdown
  STDMETHODIMP GetShutdownStatus(MFSHUTDOWN_STATUS *pStatus);

  STDMETHODIMP Shutdown();

 private:
  HRESULT CreateOutputType(Microsoft::WRL::ComPtr<IMFMediaType> &outType);
  void ParseALACBox();
  unsigned int GetOutBufferSize();
  int GetNumInputFramesToBuffer();

  Microsoft::WRL::ComPtr<IMFSample> DecodeBuffer(const Microsoft::WRL::ComPtr<IMFSample> &buffer);
  void FlushBuffers();
  bool ShouldRequestMoreInput();
  int FramesPending();

  void RequestInput();
  void NotifyOutput();
  void NotifyDrainComplete();

  void StartEventLoop();

  MediaInfo mediaInfo;
  Microsoft::WRL::ComPtr<IMFMediaType> m_inputType;
  Microsoft::WRL::ComPtr<IMFMediaType> m_outputType;

  int m_inputByteCount = 0;
  bool m_canRequestInput = false;
  bool m_isDraining = false;

  int m_numExpectedInputRequests = 0;
  int m_numExpectedOutputRequests = 0;
  int m_framesReceived = 0;
  int m_framesSent = 0;
  Util::Subscription m_onResultsReadySub;

  std::recursive_mutex m_syncLock;

  bool m_isShutdown = false;
  bool m_isShuttingDown = false;
  std::future<void> m_shutdownTask;

  Microsoft::WRL::ComPtr<IMFAttributes> m_attributes;

  Util::AsyncProcessor<Microsoft::WRL::ComPtr<IMFSample>, Microsoft::WRL::ComPtr<IMFSample>> m_processor;
  ALACDecoder m_decoder;
  Util::WorkBuffer<Microsoft::WRL::ComPtr<IMFSample>> m_outputBuffer;
  Util::WorkBuffer<Microsoft::WRL::ComPtr<IMFMediaEvent>> m_eventQueue;
  Util::WorkBuffer<std::pair<IMFAsyncCallback *, IUnknown *>> m_eventListenerQueue;  // really this queue ought to have
                                                                                     // only 1 element at a time, but
                                                                                     // this makes the logic much easier
  std::future<void> m_eventQueueTask;
};
