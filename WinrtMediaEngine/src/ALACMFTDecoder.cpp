#include <functional>
#include <wrl\module.h>
#include <Mferror.h>
#include "mfapi.h"

#include "ALACBitUtilities.h"
#include "ALACMFTDecoder.hpp"
#include "Arc_Assert.hpp"
#include "InMemoryStream.hpp"
#include "MPEG4_Parser.hpp"
#include "ScopeGuard.hpp"

using namespace Microsoft::WRL;

#define VERBOSE 1

// 261E9D83-9529-4B8F-A111-8B9C950A81A9
GUID ALAC_COOKIE = { 0x261E9D83, 0x9529, 0x4B8F, 0xA1, 0x11, 0x8B, 0x9C, 0x95, 0x0A, 0x81, 0xA9 };
const unsigned int INPUT_BUFFER_SIZE_LIMIT = 5 << 20; // About 5 Megabytes
const int TARGET_TIME_TO_BUFFER = 20; // seconds
const int NUM_DECODE_WORKERS = 1;

ActivatableClass(ALACMFTDecoder);

#define CHECK_NULL(val)      \
	if((val) == nullptr)     \
				{                        \
		return E_INVALIDARG; \
				}

#define CHECK_SHUTDOWN                   \
	if(m_isShuttingDown || m_isShutdown) \
				{                                \
		return MF_E_SHUTDOWN;            \
				}

//std::lock_guard<decltype(m_locker)> locker(m_locker);
#define SYNC_LOCK std::lock_guard<decltype(m_syncLock)> lock(m_syncLock);
	
#define CHECK_LOCK SYNC_LOCK // ARC_ASSERT(m_locker.owns_lock())

// Note: this refers to the aspect of async MFT's being unlocked for use
// Not to the concept of a lock for synchronization purposes.
// See: https://msdn.microsoft.com/en-us/library/windows/desktop/dd317909(v=vs.85).aspx
#define CHECK_UNLOCKED                                                                      \
	if(m_attributes == nullptr)                                                             \
		{                                                                                   \
		OutputDebugStringA("AXA : LOCKED"); \
		return MF_E_TRANSFORM_ASYNC_LOCKED;                                                 \
	}                                                                                       \
	UINT32 isUnlocked = 0;                                                                  \
	HRESULT unlockedHR = m_attributes->GetUINT32(MF_TRANSFORM_ASYNC_UNLOCK, &isUnlocked);   \
	if (FAILED(unlockedHR) || isUnlocked != TRUE)                                           \
	{                                                                                       \
		OutputDebugStringA("AXA : LOCKED"); \
		return MF_E_TRANSFORM_ASYNC_LOCKED;                                                 \
	} \
	OutputDebugStringA("AXB : UNLOCKED");

ALACMFTDecoder::ALACMFTDecoder()
	: m_avgBytesPerSec(0)
	, m_bitRate(0)
	, m_cookieBlob(nullptr)
	, m_channelCount(0)
	, m_samplesPerSecond(0)
	, m_bitsPerSample(0)
	, m_outputType(nullptr)
	, m_inputType(nullptr)
	, m_isOutFrameReady(false)
	, m_samplesAvailable(0)
	, m_hasEnoughInput(false)
	, m_syncLock()
	, m_attributes(nullptr)
	, m_inputByteCount(0)
	, m_processor(nullptr)
	, m_canRequestInput(false)
{
	ResetProcessor();
	StartEventLoop();
}

ALACMFTDecoder::~ALACMFTDecoder()
{
	if (m_shutdownTask.valid())
	{
		m_shutdownTask.wait();
	}
	m_cookieBlob = nullptr;
}

// IMediaExtension methods

//-------------------------------------------------------------------
// Name: SetProperties
// Sets the configuration of the decoder
//-------------------------------------------------------------------
IFACEMETHODIMP ALACMFTDecoder::SetProperties(ABI::Windows::Foundation::Collections::IPropertySet *pConfiguration)
{
	return S_OK;
}

// IMFTransform methods. Refer to the Media Foundation SDK documentation for details.

//-------------------------------------------------------------------
// Name: GetStreamLimits
// Returns the minimum and maximum number of streams.
//-------------------------------------------------------------------

HRESULT ALACMFTDecoder::GetStreamLimits(
	DWORD   *pdwInputMinimum,
	DWORD   *pdwInputMaximum,
	DWORD   *pdwOutputMinimum,
	DWORD   *pdwOutputMaximum
	)
{
	CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	if ((pdwInputMinimum == nullptr) ||
		(pdwInputMaximum == nullptr) ||
		(pdwOutputMinimum == nullptr) ||
		(pdwOutputMaximum == nullptr))
	{
		return E_POINTER;
	}

	ARC_FAIL("TODO::JT");

	// This MFT has a fixed number of streams.
	*pdwInputMinimum = 1;
	*pdwInputMaximum = 1;
	*pdwOutputMinimum = 1;
	*pdwOutputMaximum = 1;

	return S_OK;
}


//-------------------------------------------------------------------
// Name: GetStreamCount
// Returns the actual number of streams.
//-------------------------------------------------------------------

HRESULT ALACMFTDecoder::GetStreamCount(
	DWORD   *pcInputStreams,
	DWORD   *pcOutputStreams)
{
	CHECK_SHUTDOWN;

	if ((pcInputStreams == nullptr) || (pcOutputStreams == nullptr))
	{
		return E_POINTER;
	}

	*pcInputStreams = 1;
	*pcOutputStreams = 1;

	return S_OK;
}

//-------------------------------------------------------------------
// Name: GetStreamIDs
// Returns stream IDs for the input and output streams.
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::GetStreamIDs(
	DWORD   dwInputIDArraySize,
	DWORD   *pdwInputIDs,
	DWORD   dwOutputIDArraySize,
	DWORD   *pdwOutputIDs
	)
{
	if (dwInputIDArraySize < 1 || dwOutputIDArraySize < 1)
	{
		return MF_E_BUFFERTOOSMALL;
	}
	// apparently this causes the IMFMediaEngine to fail despite the documentation stating otherwise
	//CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	return E_NOTIMPL;
	
	// there is only 1 input stream and 1 output stream, therefore all streams have an id of 0
	// basically I don't have to worry about identifying them
	pdwInputIDs[0] = 0;
	pdwOutputIDs[0] = 0;

	return S_OK;
}

//-------------------------------------------------------------------
// Name: GetInputStreamInfo
// Returns information about an input stream.
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::GetInputStreamInfo(
	DWORD                     dwInputStreamID,
	MFT_INPUT_STREAM_INFO *   pStreamInfo
	)
{
	CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	ARC_FAIL("TODO::JT");
	if (pStreamInfo == nullptr)
	{
		return E_POINTER;
	}

	return S_OK;
}

//-------------------------------------------------------------------
// Name: GetOutputStreamInfo
// Returns information about an output stream.
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::GetOutputStreamInfo(
	DWORD                     dwOutputStreamID,
	MFT_OUTPUT_STREAM_INFO *  pStreamInfo
	)
{
	CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	if (pStreamInfo == nullptr)
	{
		return E_POINTER;
	}

	pStreamInfo->dwFlags = MFT_OUTPUT_STREAM_WHOLE_SAMPLES | MFT_OUTPUT_STREAM_FIXED_SAMPLE_SIZE | MFT_OUTPUT_STREAM_PROVIDES_SAMPLES;
	pStreamInfo->cbSize = GetOutBufferSize();
	pStreamInfo->cbAlignment = 0;

	return S_OK;
}

HRESULT ALACMFTDecoder::GetAttributes(IMFAttributes** pAttributes)
{
	HRESULT hr = S_OK;

	OutputDebugStringA("----- Creating attributes...\n");
	if (m_attributes == nullptr)
	{
		hr = MFCreateAttributes(&m_attributes, 3);
		ARC_ThrowIfFailed(hr);
	}

	hr = m_attributes->SetUINT32(MF_TRANSFORM_ASYNC, TRUE);
	ARC_ThrowIfFailed(hr);

	hr = m_attributes->SetUINT32(MFT_SUPPORT_DYNAMIC_FORMAT_CHANGE, TRUE);
	ARC_ThrowIfFailed(hr);

	*pAttributes = m_attributes.Get();

	return hr;
}

//-------------------------------------------------------------------
// Name: GetInputStreamAttributes
// Returns stream-level attributes for an input stream.
//-------------------------------------------------------------------

HRESULT ALACMFTDecoder::GetInputStreamAttributes(
	DWORD           dwInputStreamID,
	IMFAttributes** ppAttributes)
{
	//CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	OutputDebugStringA("--------GetInputStreamAttributes...");

	// This MFT does not support any attributes, so the method is not implemented.
	return E_NOTIMPL;
}

//-------------------------------------------------------------------
// Name: GetOutputStreamAttributes
// Returns stream-level attributes for an output stream.
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::GetOutputStreamAttributes(
	DWORD           dwOutputStreamID,
	IMFAttributes** ppAttributes
	)
{
	CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	OutputDebugStringA("--------GetOutputStreamAttributes...");

	return E_NOTIMPL;
}



//-------------------------------------------------------------------
// Name: DeleteInputStream
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::DeleteInputStream(DWORD dwStreamID)
{
	CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	ARC_FAIL("TODO::JT");
	// This MFT has a fixed number of input streams, so the method is not implemented.
	return E_NOTIMPL;
}

//-------------------------------------------------------------------
// Name: AddInputStreams
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::AddInputStreams(
	DWORD   cStreams,
	DWORD*  adwStreamIDs)
{
	CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	ARC_FAIL("TODO::JT");
	// This MFT has a fixed number of output streams, so the method is not implemented.
	return E_NOTIMPL;
}

//-------------------------------------------------------------------
// Name: GetInputAvailableType
// Description: Return a preferred input type.
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::GetInputAvailableType(
	DWORD           dwInputStreamID,
	DWORD           dwTypeIndex,
	IMFMediaType**  ppType)
{
	// IMFMediaEngine fails if we check for unlock here, However the docs say we should not proceed without the check...
	// CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	ARC_FAIL("TODO::JT");
	return MF_E_NO_MORE_TYPES;
}

//-------------------------------------------------------------------
// Name: GetOutputAvailableType
// Description: Return a preferred output type.
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::GetOutputAvailableType(
	DWORD           dwOutputStreamID,
	DWORD           dwTypeIndex, // 0-based
	IMFMediaType    **ppType)
{
	CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	// we only have one type!
	if (dwTypeIndex > 0)
	{
		return MF_E_NO_MORE_TYPES;
	}

	ComPtr<IMFMediaType> spOutputType;
	auto hr = CreateOutputType(spOutputType);
	*ppType = spOutputType.Detach();
	
	return hr;
}

//-------------------------------------------------------------------
// Name: SetInputType
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::SetInputType(
	DWORD           dwInputStreamID,
	IMFMediaType    *pType, // Can be nullptr to clear the input type.
	DWORD           dwFlags
	)
{
	//CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	HRESULT hr = S_OK;

	try
	{
		// essential values will throw an exception while secondary values will
		// just assert
		hr = pType->GetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, &m_avgBytesPerSec);
		ARC_ASSERT(hr == S_OK);

		hr = pType->GetUINT32(MF_MT_AVG_BITRATE, &m_bitRate);
		ARC_ASSERT(hr == S_OK);

		hr = pType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &m_samplesPerSecond);
		ARC_ASSERT(hr == S_OK);

		hr = pType->GetAllocatedBlob(ALAC_COOKIE, &m_cookieBlob, &m_cookieBlobSize);
		ARC_ThrowIfFailed(hr);

		hr = pType->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &m_channelCount);
		ARC_ThrowIfFailed(hr);
		
		hr = pType->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &this->m_bitsPerSample);
		ARC_ThrowIfFailed(hr);

		m_inputType = pType;

		ParseALACBox();
	}
	catch (Platform::Exception^ ex)
	{
		ARC_FAIL("Something went wrong!");
		hr = MF_E_INVALIDMEDIATYPE;
	}

	return hr;
}

void ALACMFTDecoder::ParseALACBox()
{
	using namespace Arcusical::MPEG4;

	Util::InMemoryStream stream(m_cookieBlob, m_cookieBlobSize);
	MPEG4_Parser parser;
	
	auto mpegSong = parser.ReadAndParseFromStream(stream);
	auto tree = mpegSong->GetTree();

	auto boxes = tree->GetBoxes();
	ARC_ASSERT_MSG(boxes.size() == 1, "Only one box was expected!");

	if (boxes.size() > 0)
	{
		auto stsdBox = boxes[0];
		auto children = stsdBox->GetChildren();

		ARC_ASSERT_MSG(children.size() == 1, "Expected only 1 box!");
		if (children.size() > 0)
		{
			m_alacBox = std::dynamic_pointer_cast<Alac>(children[0]);
			ARC_ASSERT(m_alacBox != nullptr);
			
			m_bitsPerSample = m_alacBox->GetSampleSize();
			m_samplesPerFrame = m_alacBox->GetSamplePerFrame();
		}
	}
}

unsigned int ALACMFTDecoder::GetOutBufferSize()
{
	return m_alacBox->GetSamplePerFrame() * m_channelCount * m_bitsPerSample / 8;
}

int ALACMFTDecoder::GetNumInputFramesToBuffer()
{
	auto bytesNeeded = m_avgBytesPerSec * TARGET_TIME_TO_BUFFER;
	auto bytesAllowed = std::min(bytesNeeded, INPUT_BUFFER_SIZE_LIMIT);
	auto bytesPerSample = m_bitsPerSample / 8;
	auto bytesPerFrame = bytesPerSample * m_samplesPerFrame;

	auto framesAllowed = bytesNeeded / bytesPerFrame;

	return framesAllowed;
}

//-------------------------------------------------------------------
// Name: SetOutputType
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::SetOutputType(
	DWORD           dwOutputStreamID,
	IMFMediaType    *pType, // Can be nullptr to clear the output type.
	DWORD           dwFlags)
{
	CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	HRESULT hr = S_OK;
	m_outputType = pType;
	return hr;
}

//-------------------------------------------------------------------
// Name: GetInputCurrentType
// Description: Returns the current input type.
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::GetInputCurrentType(
	DWORD           dwInputStreamID,
	IMFMediaType    **ppType)
{
	CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	if (ppType == nullptr)
	{
		return E_POINTER;
	}

	auto hr = MFCreateMediaType(ppType);
	ARC_ThrowIfFailed(hr);

	hr = m_inputType->CopyAllItems(*ppType);
	ARC_ThrowIfFailed(hr);

	return hr;
}

//-------------------------------------------------------------------
// Name: GetOutputCurrentType
// Description: Returns the current output type.
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::GetOutputCurrentType(
	DWORD           dwOutputStreamID,
	IMFMediaType    **ppType)
{
	CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	if (ppType == nullptr)
	{
		return E_POINTER;
	}

	ComPtr<IMFMediaType> outType;
	auto hr = CreateOutputType(outType);
	*ppType = outType.Detach();

	return S_OK;
}

//-------------------------------------------------------------------
// Name: GetInputStatus
// Description: Query if the MFT is accepting more input.
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::GetInputStatus(
	DWORD           dwInputStreamID,
	DWORD           *pdwFlags)
{
	CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	if (pdwFlags == nullptr)
	{
		return E_POINTER;
	}

	ARC_FAIL("TODO::JT");
	return S_OK;
}

//-------------------------------------------------------------------
// Name: GetOutputStatus
// Description: Query if the MFT can produce output.
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::GetOutputStatus(DWORD *pdwFlags)
{
	CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	if (pdwFlags == nullptr)
	{
		return E_POINTER;
	}

	ARC_FAIL("TODO::JT");
	return S_OK;
}

//-------------------------------------------------------------------
// Name: SetOutputBounds
// Sets the range of time stamps that the MFT will output.
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::SetOutputBounds(
	LONGLONG        /*hnsLowerBound*/,
	LONGLONG        /*hnsUpperBound*/)
{
	CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	ARC_FAIL("TODO::JT");
	// Implementation of this method is optional.
	return E_NOTIMPL;
}

//-------------------------------------------------------------------
// Name: ProcessEvent
// Sends an event to an input stream.
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::ProcessEvent(
	DWORD              /*dwInputStreamID*/,
	IMFMediaEvent      * /*pEvent */)
{
	CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	// This MFT does not handle any stream events, so the method can
	// return E_NOTIMPL. This tells the pipeline that it can stop
	// sending any more events to this MFT.
	// ARC_FAIL("TODO::JT");
	return E_NOTIMPL;
}

//-------------------------------------------------------------------
// Name: ProcessMessage
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::ProcessMessage(
	MFT_MESSAGE_TYPE    eMessage,
	ULONG_PTR           ulParam)
{
	SYNC_LOCK;
	CHECK_UNLOCKED;
	CHECK_SHUTDOWN;

	HRESULT hr = S_OK;

	switch (eMessage)
	{
	case MFT_MESSAGE_NOTIFY_START_OF_STREAM:
		m_canRequestInput = true;
		m_isDraining = false;
		ResetProcessor();
		RequestInput();
		break;
	case MFT_MESSAGE_NOTIFY_BEGIN_STREAMING:
		// Do nothing, this is just a heads up for us
		break;
	case MFT_MESSAGE_COMMAND_DRAIN:
		m_canRequestInput = false;
		m_isDraining = true;
		break;
	case MFT_MESSAGE_COMMAND_FLUSH:
		m_canRequestInput = false;
		FlushBuffers();
		break;
	case MFT_MESSAGE_NOTIFY_END_STREAMING:
		// Don't do anything in this case
		break;
	case MFT_MESSAGE_NOTIFY_END_OF_STREAM:
		m_canRequestInput = false;
		break;
	default:
		ARC_FAIL("Unhanded message");
	}

	return hr;
}

//-------------------------------------------------------------------
// Name: ProcessInput
// Description: Process an input sample.
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::ProcessInput(
	DWORD               dwInputStreamID,
	IMFSample           *pSample,
	DWORD               dwFlags)
{
	SYNC_LOCK;
	CHECK_UNLOCKED;
	CHECK_SHUTDOWN;
	CHECK_NULL(pSample);

	if (m_numExpectedInputRequests == 0)
	{
		ARC_FAIL("I don't think this should actually occur");
		return E_UNEXPECTED;
	}

	--m_numExpectedInputRequests;
	++m_framesReceived;

	DWORD bufferLength = 0;
	auto hr = pSample->GetTotalLength(&bufferLength);
	ARC_ThrowIfFailed(hr);

	m_inputByteCount += bufferLength;

	m_processor->AddItem(std::move(pSample));

	RequestInput();
	
	return hr;
}

//-------------------------------------------------------------------
// Name: ProcessOutput
// Description: Process an output sample.
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::ProcessOutput(
	DWORD                   dwFlags,
	DWORD                   cOutputBufferCount,
	MFT_OUTPUT_DATA_BUFFER  *pOutputSamples, // one per stream
	DWORD                   *pdwStatus)
{
	CHECK_UNLOCKED;
	CHECK_SHUTDOWN;
	ARC_ASSERT(cOutputBufferCount > 0);
	CHECK_NULL(pOutputSamples);
	ARC_ASSERT_MSG(pOutputSamples->pSample == nullptr, "We are allocating the samples, there should not be any provided samples!");

	if (m_numExpectedOutputRequests == 0)
	{
		ARC_FAIL("I don't think this should actually occur");
		return E_UNEXPECTED;
	}

	--m_numExpectedOutputRequests;
	++m_framesSent;
	RequestInput();
	if (m_isDraining && FramesPending() == 0)
	{
		NotifyDrainComplete();
	}

	ARC_ASSERT_MSG(m_processor->HasResult(), "Received a request for output when none was available!");
	auto result = m_processor->GetNextResult();
	pOutputSamples->pSample = result.Detach();
	return S_OK;
}

HRESULT ALACMFTDecoder::CreateOutputType(Microsoft::WRL::ComPtr<IMFMediaType>& spOutputType)
{
	HRESULT hr = S_OK;

	try
	{
		hr = MFCreateMediaType(&spOutputType);
		ARC_ThrowIfFailed(hr);

		hr = spOutputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		ARC_ThrowIfFailed(hr);

		hr = spOutputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
		ARC_ThrowIfFailed(hr);

		hr = spOutputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, m_channelCount);
		ARC_ThrowIfFailed(hr);

		hr = spOutputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, m_samplesPerSecond);
		ARC_ThrowIfFailed(hr);
		
		hr = spOutputType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, m_channelCount * m_bitsPerSample / 8);
		ARC_ThrowIfFailed(hr);

		hr = spOutputType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, m_samplesPerSecond * m_bitsPerSample * m_channelCount / 8);
		ARC_ThrowIfFailed(hr);

		hr = spOutputType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, m_bitsPerSample);
		ARC_ThrowIfFailed(hr);

		hr = spOutputType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
		ARC_ThrowIfFailed(hr);

		GUID iCantRemembeWhatThisis = { 0x494bbcf7, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc };
		hr = spOutputType->SetUINT32(iCantRemembeWhatThisis, TRUE);
		ARC_ThrowIfFailed(hr);
	}
	catch (Platform::Exception^ ex)
	{
		ARC_FAIL("Something went wrong!");
	}

	return hr;
}

STDMETHODIMP ALACMFTDecoder::BeginGetEvent(IMFAsyncCallback *pCallback, IUnknown *punkState)
{
	SYNC_LOCK;
	CHECK_SHUTDOWN;
	CHECK_NULL(pCallback);

	HRESULT hr = S_OK;
	if (m_eventListenerQueue.ResultsAvailable())
	{
		// we have a pending listener and the interface does not allow multiple.
		// we need to figure out what type it is and return the appropriate error
		// Also, we need to remember to put the callback into the queue when we are done
		// with it.
		auto eventListener = m_eventListenerQueue.GetNextNoWait();
		if (eventListener)
		{
			if (eventListener->first == pCallback)
			{
				// MF_E means same callback different data, MF_S means same callback same data
				hr = (punkState == eventListener->second) ? MF_E_MULTIPLE_BEGIN : MF_S_MULTIPLE_BEGIN;
			}
			else
			{
				hr = MF_E_MULTIPLE_SUBSCRIBERS;
			}

			m_eventListenerQueue.Append(*eventListener);
		}
	}

	// Done handling possibly bad call states... now on to the real work
	ARC_ASSERT(SUCCEEDED(hr));
	if (SUCCEEDED(hr))
	{
		m_eventListenerQueue.Append(std::make_pair(pCallback, punkState));
	}
	
	return hr;
}

STDMETHODIMP ALACMFTDecoder::EndGetEvent(IMFAsyncResult *pResult, IMFMediaEvent **ppEvent)
{
	CHECK_SHUTDOWN;
	CHECK_NULL(ppEvent);
	CHECK_LOCK;

	ComPtr<IUnknown> IUnknownEvent;
	auto hr = pResult->GetObject(&IUnknownEvent);
	ARC_ThrowIfFailed(hr);

	ComPtr<IMFMediaEvent> event;
	hr = IUnknownEvent.As(&event);
	ARC_ThrowIfFailed(hr);

	*ppEvent = event.Detach();

	OutputDebugStringA("End get event!\n");

	return S_OK;
}

STDMETHODIMP ALACMFTDecoder::GetEvent(DWORD dwFlags, IMFMediaEvent **ppEvent)
{
	SYNC_LOCK;
	CHECK_SHUTDOWN;
	ARC_ASSERT(ppEvent != nullptr);
	CHECK_NULL(ppEvent); // Assert is dev sanity check, the other check is to handle the runtime issue gracefully.

	if (m_eventListenerQueue.ResultsPending())
	{
		return MF_E_MULTIPLE_SUBSCRIBERS;
	}

	HRESULT hr = S_OK;
	boost::optional<ComPtr<IMFMediaEvent>> event;

	if (dwFlags & MF_EVENT_FLAG_NO_WAIT)
	{
		event = m_eventQueue.GetNextNoWait();
	}
	else
	{
		event = m_eventQueue.GetNext();
	}

	if (event)
	{
		*ppEvent = event->Get();
		hr = S_OK;
	}
	else
	{
		if (dwFlags & MF_EVENT_FLAG_NO_WAIT)
		{
			hr = MF_E_NO_EVENTS_AVAILABLE;
		}
		else
		{
			// we blocked and waited... but we still didn't get anything.
			// most likely we shut down, if we didn't shut down we are now in an odd state
			// check shutdown will return if we are shutdown.
			CHECK_SHUTDOWN;
			ARC_FAIL("This state shouldn't be possible!");
		}
	}

	return hr;
}

STDMETHODIMP ALACMFTDecoder::QueueEvent(MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT *pvValue)
{
	SYNC_LOCK;
	CHECK_SHUTDOWN;

	ComPtr<IMFMediaEvent> event;
	auto hr = MFCreateMediaEvent(met, guidExtendedType, hrStatus, pvValue, &event);
	ARC_ThrowIfFailed(hr);

	m_eventQueue.Append(event);

	return hr;
}

STDMETHODIMP ALACMFTDecoder::GetShutdownStatus(MFSHUTDOWN_STATUS *pStatus)
{
	SYNC_LOCK;
	CHECK_NULL(pStatus);

	HRESULT hr = S_OK;

	if (m_isShutdown)
	{
		*pStatus = MFSHUTDOWN_COMPLETED;
	}
	else if (m_isShuttingDown)
	{
		*pStatus = MFSHUTDOWN_INITIATED;
	}
	else
	{
		hr = MF_E_INVALIDREQUEST;
	}

	return hr;
}

STDMETHODIMP ALACMFTDecoder::Shutdown()
{
	SYNC_LOCK;
	ARC_ASSERT(!m_isShutdown);

	m_isShuttingDown = true;
	m_shutdownTask = std::async([this]()
	{
		m_onWorkReadySub.Unsubscribe();
		m_eventListenerQueue.Complete();
		m_eventQueue.Complete();
		m_eventQueueTask.wait();
		m_processor->StopWork();
		m_isShutdown = true;
	});

	return S_OK;
}

void ALACMFTDecoder::RequestInput()
{
	CHECK_LOCK;

	if (!ShouldRequestMoreInput())
	{
		return;
	}

	ComPtr<IMFMediaEvent> inputEvent = nullptr;
	auto hr = MFCreateMediaEvent(METransformNeedInput, GUID_NULL, S_OK, NULL, &inputEvent);
	ARC_ThrowIfFailed(hr);

	//hr = inputEvent->SetUINT32(MF_EVENT_MFT_INPUT_STREAM_ID, 0);
	ARC_ThrowIfFailed(hr);

	++m_numExpectedInputRequests;
	m_eventQueue.Append(inputEvent);
}

void ALACMFTDecoder::NotifyOutput()
{
	ComPtr<IMFMediaEvent> outputEvent = nullptr;
	auto hr = MFCreateMediaEvent(METransformHaveOutput, GUID_NULL, S_OK, NULL, &outputEvent);
	ARC_ThrowIfFailed(hr);

	++m_numExpectedOutputRequests;
	m_eventQueue.Append(outputEvent);
}

void ALACMFTDecoder::NotifyDrainComplete()
{
	ComPtr<IMFMediaEvent> drainEvent = nullptr;
	auto hr = MFCreateMediaEvent(METransformDrainComplete, GUID_NULL, S_OK, NULL, &drainEvent);
	ARC_ThrowIfFailed(hr);

	m_eventQueue.Append(drainEvent);
}

void ALACMFTDecoder::StartEventLoop()
{
	m_eventQueueTask = std::async([this]()
	{
		boost::optional<ComPtr<IMFMediaEvent>> event = boost::none;
		boost::optional<std::pair<IMFAsyncCallback*, IUnknown*>> callback = boost::none;
		while (!m_isShuttingDown)
		{
			if (!event)
			{
				// blocks and waits for the next event
				event = m_eventQueue.GetNext();
			}
			if (!callback)
			{
				// blocks and waits for a callback
				callback = m_eventListenerQueue.GetNext();
			}
			if (event && callback)
			{
				ComPtr<IMFAsyncResult> asyncResult = nullptr;
				HRESULT hr = MFCreateAsyncResult(event->Get(), callback->first, callback->second, &asyncResult);
				ARC_ThrowIfFailed(hr);

				hr = callback->first->Invoke(asyncResult.Get());
				ARC_ThrowIfFailed(hr);

				event = boost::none;
				callback = boost::none;
			}
		}
	});
}

ComPtr<IMFSample> ALACMFTDecoder::DecodeBuffer(const ComPtr<IMFSample>& pSample)
{
	ComPtr<IMFMediaBuffer> inputMediaBuffer = nullptr;
	auto hr = pSample->ConvertToContiguousBuffer(&inputMediaBuffer);
	ARC_ThrowIfFailed(hr);

	// TODO::JT how expensive is setting up the decoder? Is it feasible to do for each and every frame?
	ALACDecoder decoder;
	decoder.Init(m_cookieBlob + 52, m_cookieBlobSize - 52);

	BYTE* inBuffer = nullptr;
	DWORD inputBufferLength = 0;
	BYTE* outBuffer = nullptr;
	DWORD outputBufferLength = 0;

	auto outBufferSize = GetOutBufferSize();
	ComPtr<IMFMediaBuffer> outputMediaBuffer = nullptr;
	hr = MFCreateMemoryBuffer(outBufferSize, &outputMediaBuffer);
	ARC_ThrowIfFailed(hr);
	hr = outputMediaBuffer->SetCurrentLength(outBufferSize);
	ARC_ThrowIfFailed(hr);

	// scope is to unlock the media buffers as soon as we are done with them
	{
		hr = inputMediaBuffer->Lock(&inBuffer, NULL, &inputBufferLength);
		ARC_ThrowIfFailed(hr);
		Util::ScopeGuard<std::function<void()>> inputBufferUnlocker([&inputMediaBuffer]()
		{
			auto hr = inputMediaBuffer->Unlock();
			ARC_ASSERT(SUCCEEDED(hr));
		});

		BitBuffer bitsWrapper;
		BitBufferInit(&bitsWrapper, inBuffer, inputBufferLength);

		hr = outputMediaBuffer->Lock(&outBuffer, NULL, &outputBufferLength);
		ARC_ThrowIfFailed(hr);
		Util::ScopeGuard<std::function<void()>> outputBufferUnlocker([&outputMediaBuffer]()
		{
			auto hr = outputMediaBuffer->Unlock();
			ARC_ASSERT(SUCCEEDED(hr));
		});
		ARC_ASSERT(outputBufferLength == outputBufferLength); // one is what we told it to be, the other is what it said it is

		auto decodeResult = decoder.Decode(&bitsWrapper, outBuffer, m_alacBox->GetSamplePerFrame(), m_channelCount, &m_samplesAvailable);
		ARC_ASSERT(decodeResult == ALAC_noErr);
	}
	
	// we are going to reuse the provided IMFSample. However, to do so requires that we strip out the buffers and replace
	// them with our own.
	hr = pSample->RemoveAllBuffers();
	ARC_ThrowIfFailed(hr);

	hr = pSample->AddBuffer(outputMediaBuffer.Get());
	ARC_ThrowIfFailed(hr);

	return pSample;
}

int ALACMFTDecoder::FramesPending()
{
	return m_framesReceived - m_framesSent;
}

bool ALACMFTDecoder::ShouldRequestMoreInput()
{
	return m_canRequestInput && FramesPending() < GetNumInputFramesToBuffer();
}

void ALACMFTDecoder::FlushBuffers()
{
	m_processor->StopWork();
	m_inputByteCount = 0;
	m_numExpectedInputRequests = 0;
	m_numExpectedOutputRequests = 0;
	m_framesReceived = 0;
	m_framesSent = 0;
}

void ALACMFTDecoder::ResetProcessor()
{
	m_onWorkReadySub.Unsubscribe();
	if (m_processor != nullptr)
	{
		m_processor->StopWork();
	}
	
	m_processor = nullptr;
	m_processor = std::make_unique<Util::ParallelOrderedProcessor<Microsoft::WRL::ComPtr<IMFSample>,
		                                                          Microsoft::WRL::ComPtr<IMFSample>>>(NUM_DECODE_WORKERS);
	m_onWorkReadySub = m_processor->OnResultReady += [this]()
	{
		NotifyOutput();
		RequestInput();
	};
	m_processor->SetWorkFunc([this](const Microsoft::WRL::ComPtr<IMFSample>& buffer)
	{
		return this->DecodeBuffer(buffer);
	});
}
