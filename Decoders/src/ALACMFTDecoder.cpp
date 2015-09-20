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

ActivatableClass(ALACMFTDecoder);

#define CHECK_NULL(val)      \
	if((val) == nullptr)     \
	{                        \
		return E_INVALIDARG; \
	}

#define CHECK_SHUTDOWN                   \
	if(m_isShuttingDown || m_isShutdown) \
	{                                    \
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
	{                                                                                       \
		return MF_E_TRANSFORM_ASYNC_LOCKED;                                                 \
	}                                                                                       \
	UINT32 isUnlocked = 0;                                                                  \
	HRESULT unlockedHR = m_attributes->GetUINT32(MF_TRANSFORM_ASYNC_UNLOCK, &isUnlocked);   \
	if (FAILED(unlockedHR) || isUnlocked != TRUE)                                           \
	{                                                                                       \
		return MF_E_TRANSFORM_ASYNC_LOCKED;                                                 \
	}

ALACMFTDecoder::ALACMFTDecoder()
	: m_outputType(nullptr)
	, m_inputType(nullptr)
	, m_syncLock()
	, m_attributes(nullptr)
	, m_inputByteCount(0)
	, m_canRequestInput(false)
{
	OutputDebugStringA("New Song !!! axa\n");
	StartEventLoop();
	m_processor.Connect(&m_outputBuffer);
	m_onResultsReadySub = m_outputBuffer.ItemAvailable += [this]()
	{
		this->NotifyOutput();
	};
	m_processor.SetProcessor([this](const Microsoft::WRL::ComPtr<IMFSample>& buffer)
	{
		return this->DecodeBuffer(buffer);
	});
}

ALACMFTDecoder::~ALACMFTDecoder()
{
	OutputDebugStringA("Deleting Decoder\n");
	if (m_shutdownTask.valid())
	{
		m_shutdownTask.wait();
	}
	mediaInfo.cookieBlob = nullptr;
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

	if (m_attributes == nullptr)
	{
		OutputDebugStringA("----- Creating attributes...\n");
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

	OutputDebugStringA("--------GetInputStreamAttributes...\n");

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

	OutputDebugStringA("--------GetOutputStreamAttributes...\n");

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
		hr = pType->GetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, &mediaInfo.avgBytesPerSec);
		ARC_ASSERT(hr == S_OK);

		hr = pType->GetUINT32(MF_MT_AVG_BITRATE, &mediaInfo.bitRate);
		ARC_ASSERT(hr == S_OK);

		hr = pType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &mediaInfo.samplesPerSecond);
		ARC_ASSERT(hr == S_OK);

		hr = pType->GetAllocatedBlob(ALAC_COOKIE, &mediaInfo.cookieBlob, &mediaInfo.cookieBlobSize);
		ARC_ThrowIfFailed(hr);

		hr = pType->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &mediaInfo.channelCount);
		ARC_ThrowIfFailed(hr);
		
		hr = pType->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &mediaInfo.bitsPerSample);
		ARC_ThrowIfFailed(hr);

		m_inputType = pType;

		ParseALACBox();
		m_decoder.Init(mediaInfo.cookieBlob + 52, mediaInfo.cookieBlobSize - 52);
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

	Util::InMemoryStream stream(mediaInfo.cookieBlob, mediaInfo.cookieBlobSize);
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
			mediaInfo.alacBox = std::dynamic_pointer_cast<Alac>(children[0]);
			ARC_ASSERT(mediaInfo.alacBox != nullptr);
			
			mediaInfo.bitsPerSample = mediaInfo.alacBox->GetSampleSize();
			mediaInfo.samplesPerFrame = mediaInfo.alacBox->GetSamplePerFrame();
		}
	}
}

unsigned int ALACMFTDecoder::GetOutBufferSize()
{
	return mediaInfo.alacBox->GetSamplePerFrame() * mediaInfo.channelCount * mediaInfo.bitsPerSample / 8;
}

int ALACMFTDecoder::GetNumInputFramesToBuffer()
{
	auto bytesNeeded = mediaInfo.avgBytesPerSec * TARGET_TIME_TO_BUFFER;
	auto bytesAllowed = std::min(bytesNeeded, INPUT_BUFFER_SIZE_LIMIT);
	auto bytesPerSample = mediaInfo.bitsPerSample / 8;
	auto bytesPerFrame = bytesPerSample * mediaInfo.samplesPerFrame;

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
		OutputDebugStringA("Start of stream\n");
		RequestInput();
		break;
	case MFT_MESSAGE_NOTIFY_BEGIN_STREAMING:
		// Do nothing, this is just a heads up for us
		OutputDebugStringA("Notify Begin Streaming\n");
		break;
	case MFT_MESSAGE_COMMAND_DRAIN:
		{
			m_canRequestInput = false;
			m_isDraining = true;
			OutputDebugStringA("Command : Drain : ");
			auto foo = std::to_string(m_numExpectedInputRequests) + ", " + std::to_string(m_numExpectedOutputRequests) + "\n";
			OutputDebugStringA(foo.c_str());
		}
		break;
	case MFT_MESSAGE_COMMAND_FLUSH:
		m_canRequestInput = false;
		FlushBuffers();
		OutputDebugStringA("Command : Flush\n");
		break;
	case MFT_MESSAGE_NOTIFY_END_STREAMING:
		// Don't do anything in this case
		OutputDebugStringA("Notify end of streaming\n");
		break;
	case MFT_MESSAGE_NOTIFY_END_OF_STREAM:
		m_canRequestInput = false;
		OutputDebugStringA("This is the end of the stream\n");
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
	OutputDebugStringA("Process Input : ");

	if (m_numExpectedInputRequests == 0)
	{
		//ARC_FAIL("I don't think this should actually occur");
		// return E_UNEXPECTED;
	}

	--m_numExpectedInputRequests;
	++m_framesReceived;

	DWORD bufferLength = 0;
	auto hr = pSample->GetTotalLength(&bufferLength);
	ARC_ThrowIfFailed(hr);

	m_inputByteCount += bufferLength;

	if (!m_processor.IsRunning())
	{
		m_processor.Start();
	}

	m_processor.Append(std::move(pSample));

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
	OutputDebugStringA("Process Output : ");

	SYNC_LOCK;
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

	RequestInput();
	--m_numExpectedOutputRequests;
	++m_framesSent;

	OutputDebugStringA("Frames pending: ");
	OutputDebugStringA(std::to_string(FramesPending()).c_str());
	OutputDebugStringA("\n");
	if (m_isDraining && FramesPending() == 0)
	{
		NotifyDrainComplete();
	}

	if (m_outputBuffer.ResultsAvailable())
	{
		auto result = m_outputBuffer.GetNext();
		ARC_ASSERT(result);
		pOutputSamples->pSample = result->Detach();
	}
	else
	{
		OutputDebugStringA("No results available\n");
		pOutputSamples->dwStatus = MFT_OUTPUT_DATA_BUFFER_NO_SAMPLE;
	}
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

		hr = spOutputType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, mediaInfo.channelCount);
		ARC_ThrowIfFailed(hr);

		hr = spOutputType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, mediaInfo.samplesPerSecond);
		ARC_ThrowIfFailed(hr);
		
		hr = spOutputType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, mediaInfo.channelCount * mediaInfo.bitsPerSample / 8);
		ARC_ThrowIfFailed(hr);

		hr = spOutputType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, mediaInfo.samplesPerSecond * mediaInfo.bitsPerSample * mediaInfo.channelCount / 8);
		ARC_ThrowIfFailed(hr);

		hr = spOutputType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, mediaInfo.bitsPerSample);
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

	MediaEventType type;
	event->GetType(&type);

	*ppEvent = event.Detach();

	OutputDebugStringA("End get event! : ");
	OutputDebugStringA(std::to_string(type).c_str());

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

	WCHAR extended[100];
	StringFromGUID2(guidExtendedType, extended, 100);

	auto foo = " : " + std::to_string(met) + "\n";
	OutputDebugStringA("Queue Event : ");
	OutputDebugString(extended);
	OutputDebugStringA(foo.c_str());

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
		m_onResultsReadySub.Unsubscribe();
		m_eventListenerQueue.Complete();
		m_eventQueue.Complete();
		m_eventQueueTask.wait();
		m_processor.DropAllWork();
		m_processor.Stop();
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
	OutputDebugStringA("Requesting Input\n");
}

void ALACMFTDecoder::NotifyOutput()
{
	ComPtr<IMFMediaEvent> outputEvent = nullptr;
	auto hr = MFCreateMediaEvent(METransformHaveOutput, GUID_NULL, S_OK, NULL, &outputEvent);
	ARC_ThrowIfFailed(hr);

	++m_numExpectedOutputRequests;
	m_eventQueue.Append(outputEvent);
	OutputDebugStringA("Notify Output\n");
}

void ALACMFTDecoder::NotifyDrainComplete()
{
	ComPtr<IMFMediaEvent> drainEvent = nullptr;
	auto hr = MFCreateMediaEvent(METransformDrainComplete, GUID_NULL, S_OK, NULL, &drainEvent);
	ARC_ThrowIfFailed(hr);

	m_eventQueue.Append(drainEvent);
	OutputDebugStringA("Drain Complete\n");
}

void ALACMFTDecoder::StartEventLoop()
{
	m_eventQueueTask = std::async([this]()
	{
		boost::optional<ComPtr<IMFMediaEvent>> event = boost::none;
		boost::optional<std::pair<IMFAsyncCallback*, IUnknown*>> callback = boost::none;
		while (!m_isShuttingDown)
		{
			// TODO::JT there is a race condition in this code. What can happen is that
			// an event is pulled from the queue. Then a flush occurs. Then the event is
			// passed to the listener. If this happens we get an expected call, typically
			// to ProcessOutput or ProcessInput.
			if (!callback)
			{
				// blocks and waits for a callback
				callback = m_eventListenerQueue.GetNext();
			}if (!event)
			{
				// blocks and waits for the next event
				event = m_eventQueue.GetNext();
			}
			if (event && callback)
			{
				MediaEventType type;
				(*event)->GetType(&type);
				auto foo = "Beginning event loop with : " + std::to_string(type) + "\n";
				OutputDebugStringA(foo.c_str());
				ComPtr<IMFAsyncResult> asyncResult = nullptr;
				HRESULT hr = MFCreateAsyncResult(event->Get(), callback->first, callback->second, &asyncResult);
				ARC_ThrowIfFailed(hr);

				hr = callback->first->Invoke(asyncResult.Get());
				ARC_ThrowIfFailed(hr);

				event = boost::none;
				callback = boost::none;
			}
			if (event)
			{
				ARC_FAIL("I don't think this can actually run...");
				m_eventQueue.Append(*event);
				event = boost::none;
			}
		}
	});
}

// Can't use the built in ConvertToContiguousBuffer of IMFSample because the alac code
// over reads the input buffer by 1 byte in particular cases. As such we need to very slightly
// over allocate the read buffer to avoid a segfault.
ComPtr<IMFMediaBuffer> GetContigousMediaBuffer(const ComPtr<IMFSample>& pSample)
{
	DWORD bufCount;
	auto hr = pSample->GetBufferCount(&bufCount);
	ARC_ThrowIfFailed(hr);

	ComPtr<IMFMediaBuffer> fullBuffer = nullptr;

	if (bufCount == 1)
	{
		// Never seem to need over allocation here, just going to return the buffer
		hr = pSample->GetBufferByIndex(0, &fullBuffer);
		ARC_ThrowIfFailed(hr);
		OutputDebugStringA("using 1 buf\n");
	}
	else
	{
		OutputDebugStringA("using 2 buf\n");
		DWORD totalLength = 0;
		for (DWORD index = 0; index < bufCount; ++index)
		{
			ComPtr<IMFMediaBuffer> buffer = nullptr;
			hr = pSample->GetBufferByIndex(index, &buffer);
			ARC_ThrowIfFailed(hr);

			DWORD length = 0;
			hr = buffer->GetCurrentLength(&length);
			ARC_ThrowIfFailed(hr);

			totalLength += length;
		}

		totalLength += 3;

		std::vector<unsigned char> contigousBuffer;
		contigousBuffer.resize(totalLength);
		BYTE* next = contigousBuffer.data();

		for (DWORD index = 0; index < bufCount; ++index)
		{
			ComPtr<IMFMediaBuffer> buffer = nullptr;
			hr = pSample->GetBufferByIndex(index, &buffer);
			ARC_ThrowIfFailed(hr);

			BYTE* rawBuf = nullptr;
			DWORD bufLength = 0;
			hr = buffer->Lock(&rawBuf, nullptr, &bufLength);
			ARC_ThrowIfFailed(hr);

			std::copy(rawBuf, rawBuf + bufLength, next);
			next += bufLength;

			hr = buffer->Unlock();
			ARC_ThrowIfFailed(hr);
		}

		hr = MFCreateMemoryBuffer(totalLength, &fullBuffer);
		ARC_ThrowIfFailed(hr);
		hr = fullBuffer->SetCurrentLength(totalLength);

		BYTE* rawBuf = nullptr;
		DWORD bufLength = 0;
		hr = fullBuffer->Lock(&rawBuf, nullptr, &bufLength);
		ARC_ThrowIfFailed(hr);

		std::copy(contigousBuffer.data(), contigousBuffer.data() + contigousBuffer.size(), rawBuf);

		hr = fullBuffer->Unlock();
		ARC_ThrowIfFailed(hr);
	}

	return fullBuffer;
}

ComPtr<IMFSample> ALACMFTDecoder::DecodeBuffer(const ComPtr<IMFSample>& pSample)
{
	ComPtr<IMFMediaBuffer> inputMediaBuffer = GetContigousMediaBuffer(pSample);

	BYTE* inBuffer = nullptr;
	DWORD inputBufferLength = 0;
	BYTE* outBuffer = nullptr;
	DWORD outputBufferLength = 0;

	auto hr = inputMediaBuffer->GetCurrentLength(&inputBufferLength);
	ARC_ThrowIfFailed(hr);

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
		// zero out the buffer to make sure there are no 'blips' in the buffer
		std::fill_n(outBuffer, outputBufferLength, 0xBE);
		ARC_ASSERT(outputBufferLength == outputBufferLength); // one is what we told it to be, the other is what it said it is

		uint32_t numSamplesOut = 0;
		auto decodeResult = m_decoder.Decode(&bitsWrapper, outBuffer, mediaInfo.alacBox->GetSamplePerFrame(), mediaInfo.channelCount, &numSamplesOut);
		ARC_ASSERT(decodeResult == ALAC_noErr);
		ARC_ASSERT(numSamplesOut > 0);
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
	auto res = m_framesReceived - m_framesSent;
	ARC_ASSERT(res >= 0);
	return res;
}

bool ALACMFTDecoder::ShouldRequestMoreInput()
{
	return m_canRequestInput && FramesPending() < GetNumInputFramesToBuffer();
}

void ALACMFTDecoder::FlushBuffers()
{
	m_processor.DropAllWork();
	
	m_eventQueue.RemoveIf([](const Microsoft::WRL::ComPtr<IMFMediaEvent>& event)
	{
		MediaEventType type;
		auto hr = event->GetType(&type);
		ARC_ThrowIfFailed(hr);
		return type == METransformNeedInput || type == METransformHaveOutput;
	});
	m_inputByteCount = 0;
	m_numExpectedInputRequests = 0;
	m_numExpectedOutputRequests = 0;
	m_framesReceived = 0;
	m_framesSent = 0;
}