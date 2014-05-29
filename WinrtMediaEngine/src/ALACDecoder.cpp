#include "pch.h"

#include <wrl\module.h>
#include <Mferror.h>

#include "ALACDecoder.hpp"
#include "Arc_Assert.hpp"

using namespace Microsoft::WRL;

// 261E9D83-9529-4B8F-A111-8B9C950A81A9
GUID ALAC_COOKIE = { 0x261E9D83, 0x9529, 0x4B8F, 0xA1, 0x11, 0x8B, 0x9C, 0x95, 0x0A, 0x81, 0xA9 };

ActivatableClass(ALACDecoder);

ALACDecoder::ALACDecoder()
	: m_avgBytesPerSec(0)
	, m_bitRate(0)
	, m_cookieBlob(nullptr)
	, m_channelCount(0)
	, m_samplesPerSecond(0)
	, m_bitsPerSample(0)
	, m_outputType(nullptr)
	, m_inputType(nullptr)
{
	
}

ALACDecoder::~ALACDecoder()
{
	delete(m_cookieBlob);
	m_cookieBlob = nullptr;
}

// IMediaExtension methods

//-------------------------------------------------------------------
// Name: SetProperties
// Sets the configuration of the decoder
//-------------------------------------------------------------------
IFACEMETHODIMP ALACDecoder::SetProperties(ABI::Windows::Foundation::Collections::IPropertySet *pConfiguration)
{
	return S_OK;
}

// IMFTransform methods. Refer to the Media Foundation SDK documentation for details.

//-------------------------------------------------------------------
// Name: GetStreamLimits
// Returns the minimum and maximum number of streams.
//-------------------------------------------------------------------

HRESULT ALACDecoder::GetStreamLimits(
	DWORD   *pdwInputMinimum,
	DWORD   *pdwInputMaximum,
	DWORD   *pdwOutputMinimum,
	DWORD   *pdwOutputMaximum
	)
{

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

HRESULT ALACDecoder::GetStreamCount(
	DWORD   *pcInputStreams,
	DWORD   *pcOutputStreams)
{
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
HRESULT ALACDecoder::GetStreamIDs(
	DWORD   dwInputIDArraySize,
	DWORD   *pdwInputIDs,
	DWORD   dwOutputIDArraySize,
	DWORD   *pdwOutputIDs
	)
{
	//ARC_FAIL("TODO::JT");
	// Do not need to implement, because this MFT has a fixed number of
	// streams and the stream IDs match the stream indexes.
	return E_NOTIMPL;
}

//-------------------------------------------------------------------
// Name: GetInputStreamInfo
// Returns information about an input stream.
//-------------------------------------------------------------------
HRESULT ALACDecoder::GetInputStreamInfo(
	DWORD                     dwInputStreamID,
	MFT_INPUT_STREAM_INFO *   pStreamInfo
	)
{
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
HRESULT ALACDecoder::GetOutputStreamInfo(
	DWORD                     dwOutputStreamID,
	MFT_OUTPUT_STREAM_INFO *  pStreamInfo
	)
{
	ARC_FAIL("TODO::JT");

	if (pStreamInfo == nullptr)
	{
		return E_POINTER;
	}

	return S_OK;
}

HRESULT ALACDecoder::GetAttributes(IMFAttributes** pAttributes)
{
	return E_NOTIMPL;
}

//-------------------------------------------------------------------
// Name: GetInputStreamAttributes
// Returns stream-level attributes for an input stream.
//-------------------------------------------------------------------

HRESULT ALACDecoder::GetInputStreamAttributes(
	DWORD           dwInputStreamID,
	IMFAttributes** ppAttributes)
{
	// This MFT does not support any attributes, so the method is not implemented.
	return E_NOTIMPL;
}

//-------------------------------------------------------------------
// Name: GetOutputStreamAttributes
// Returns stream-level attributes for an output stream.
//-------------------------------------------------------------------
HRESULT ALACDecoder::GetOutputStreamAttributes(
	DWORD           dwOutputStreamID,
	IMFAttributes** ppAttributes
	)
{
	return E_NOTIMPL;
}



//-------------------------------------------------------------------
// Name: DeleteInputStream
//-------------------------------------------------------------------
HRESULT ALACDecoder::DeleteInputStream(DWORD dwStreamID)
{
	ARC_FAIL("TODO::JT");
	// This MFT has a fixed number of input streams, so the method is not implemented.
	return E_NOTIMPL;
}

//-------------------------------------------------------------------
// Name: AddInputStreams
//-------------------------------------------------------------------
HRESULT ALACDecoder::AddInputStreams(
	DWORD   cStreams,
	DWORD*  adwStreamIDs)
{
	ARC_FAIL("TODO::JT");
	// This MFT has a fixed number of output streams, so the method is not implemented.
	return E_NOTIMPL;
}

//-------------------------------------------------------------------
// Name: GetInputAvailableType
// Description: Return a preferred input type.
//-------------------------------------------------------------------
HRESULT ALACDecoder::GetInputAvailableType(
	DWORD           dwInputStreamID,
	DWORD           dwTypeIndex,
	IMFMediaType**  ppType)
{
	ARC_FAIL("TODO::JT");
	return MF_E_NO_MORE_TYPES;
}

//-------------------------------------------------------------------
// Name: GetOutputAvailableType
// Description: Return a preferred output type.
//-------------------------------------------------------------------
HRESULT ALACDecoder::GetOutputAvailableType(
	DWORD           dwOutputStreamID,
	DWORD           dwTypeIndex, // 0-based
	IMFMediaType    **ppType)
{
	// we only have one type!
	if (dwTypeIndex > 0)
	{
		return MF_E_NO_MORE_TYPES;
	}

	OutputDebugStringA("axa\n");

	ComPtr<IMFMediaType> spOutputType;
	auto hr = CreateOutputType(spOutputType);
	*ppType = spOutputType.Detach();
	
	return hr;
}

//-------------------------------------------------------------------
// Name: SetInputType
//-------------------------------------------------------------------
HRESULT ALACDecoder::SetInputType(
	DWORD           dwInputStreamID,
	IMFMediaType    *pType, // Can be nullptr to clear the input type.
	DWORD           dwFlags
	)
{
	HRESULT hr = S_OK;

	OutputDebugStringA("axb\n");

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
	}
	catch (Platform::Exception^ ex)
	{
		ARC_FAIL("Something went wrong!");
		hr = MF_E_INVALIDMEDIATYPE;
	}

	return hr;
}

//-------------------------------------------------------------------
// Name: SetOutputType
//-------------------------------------------------------------------
HRESULT ALACDecoder::SetOutputType(
	DWORD           dwOutputStreamID,
	IMFMediaType    *pType, // Can be nullptr to clear the output type.
	DWORD           dwFlags)
{
	HRESULT hr = S_OK;
	m_outputType = pType;
	return hr;
}

//-------------------------------------------------------------------
// Name: GetInputCurrentType
// Description: Returns the current input type.
//-------------------------------------------------------------------
HRESULT ALACDecoder::GetInputCurrentType(
	DWORD           dwInputStreamID,
	IMFMediaType    **ppType)
{
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
HRESULT ALACDecoder::GetOutputCurrentType(
	DWORD           dwOutputStreamID,
	IMFMediaType    **ppType)
{
	if (ppType == nullptr)
	{
		return E_POINTER;
	}

	OutputDebugStringA("axc\n");

	ComPtr<IMFMediaType> outType;
	auto hr = CreateOutputType(outType);
	*ppType = outType.Detach();

	return S_OK;
}

//-------------------------------------------------------------------
// Name: GetInputStatus
// Description: Query if the MFT is accepting more input.
//-------------------------------------------------------------------
HRESULT ALACDecoder::GetInputStatus(
	DWORD           dwInputStreamID,
	DWORD           *pdwFlags)
{
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
HRESULT ALACDecoder::GetOutputStatus(DWORD *pdwFlags)
{
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
HRESULT ALACDecoder::SetOutputBounds(
	LONGLONG        /*hnsLowerBound*/,
	LONGLONG        /*hnsUpperBound*/)
{
	ARC_FAIL("TODO::JT");
	// Implementation of this method is optional.
	return E_NOTIMPL;
}

//-------------------------------------------------------------------
// Name: ProcessEvent
// Sends an event to an input stream.
//-------------------------------------------------------------------
HRESULT ALACDecoder::ProcessEvent(
	DWORD              /*dwInputStreamID*/,
	IMFMediaEvent      * /*pEvent */)
{
	// This MFT does not handle any stream events, so the method can
	// return E_NOTIMPL. This tells the pipeline that it can stop
	// sending any more events to this MFT.
	// ARC_FAIL("TODO::JT");
	return E_NOTIMPL;
}

//-------------------------------------------------------------------
// Name: ProcessMessage
//-------------------------------------------------------------------
HRESULT ALACDecoder::ProcessMessage(
	MFT_MESSAGE_TYPE    eMessage,
	ULONG_PTR           ulParam)
{
	ARC_FAIL("TODO::JT");
	//AutoLock lock(m_critSec);
	return S_OK;
}

//-------------------------------------------------------------------
// Name: ProcessInput
// Description: Process an input sample.
//-------------------------------------------------------------------
HRESULT ALACDecoder::ProcessInput(
	DWORD               dwInputStreamID,
	IMFSample           *pSample,
	DWORD               dwFlags)
{
	ARC_FAIL("TODO::JT");
	return S_OK;
}

//-------------------------------------------------------------------
// Name: ProcessOutput
// Description: Process an output sample.
//-------------------------------------------------------------------
HRESULT ALACDecoder::ProcessOutput(
	DWORD                   dwFlags,
	DWORD                   cOutputBufferCount,
	MFT_OUTPUT_DATA_BUFFER  *pOutputSamples, // one per stream
	DWORD                   *pdwStatus)
{
	ARC_FAIL("TODO::JT");
	return S_OK;
}

HRESULT ALACDecoder::CreateOutputType(Microsoft::WRL::ComPtr<IMFMediaType>& spOutputType)
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

		hr = spOutputType->SetUINT32({ 0x494bbcf7, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc }, TRUE);
		ARC_ThrowIfFailed(hr);
	}
	catch (Platform::Exception^ ex)
	{
		ARC_FAIL("Something went wrong!");
	}

	return hr;
}