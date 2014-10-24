#include "pch.h"

#include <wrl\module.h>
#include <Mferror.h>

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

ActivatableClass(ALACMFTDecoder);

ALACMFTDecoder::ALACMFTDecoder()
	: m_avgBytesPerSec(0)
	, m_bitRate(0)
	, m_cookieBlob(nullptr)
	, m_channelCount(0)
	, m_samplesPerSecond(0)
	, m_bitsPerSample(0)
	, m_outputType(nullptr)
	, m_inputType(nullptr)
	, m_outBuffer()
	, m_isOutFrameReady(false)
	, m_samplesAvailable(0)
	, m_hasEnoughInput(false)
{
	
}

ALACMFTDecoder::~ALACMFTDecoder()
{
	// TODO::JT something wrong with this..
	//delete(m_cookieBlob);
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
	//ARC_FAIL("TODO::JT");
	// Do not need to implement, because this MFT has a fixed number of
	// streams and the stream IDs match the stream indexes.
	return E_NOTIMPL;
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
	if (pStreamInfo == nullptr)
	{
		return E_POINTER;
	}

	pStreamInfo->dwFlags = MFT_OUTPUT_STREAM_WHOLE_SAMPLES | MFT_OUTPUT_STREAM_FIXED_SAMPLE_SIZE;
	pStreamInfo->cbSize = GetOutBufferSize();
	pStreamInfo->cbAlignment = 0;

	return S_OK;
}

HRESULT ALACMFTDecoder::GetAttributes(IMFAttributes** pAttributes)
{
	return E_NOTIMPL;
}

//-------------------------------------------------------------------
// Name: GetInputStreamAttributes
// Returns stream-level attributes for an input stream.
//-------------------------------------------------------------------

HRESULT ALACMFTDecoder::GetInputStreamAttributes(
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
HRESULT ALACMFTDecoder::GetOutputStreamAttributes(
	DWORD           dwOutputStreamID,
	IMFAttributes** ppAttributes
	)
{
	return E_NOTIMPL;
}



//-------------------------------------------------------------------
// Name: DeleteInputStream
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::DeleteInputStream(DWORD dwStreamID)
{
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

			m_outBuffer.resize(GetOutBufferSize());
		}
	}

	m_decoder.Init(m_cookieBlob + 52, m_cookieBlobSize - 52);
}

unsigned int ALACMFTDecoder::GetOutBufferSize()
{
	return m_alacBox->GetSamplePerFrame() * m_channelCount * m_bitsPerSample / 8;
}

//-------------------------------------------------------------------
// Name: SetOutputType
//-------------------------------------------------------------------
HRESULT ALACMFTDecoder::SetOutputType(
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
HRESULT ALACMFTDecoder::GetInputCurrentType(
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
HRESULT ALACMFTDecoder::GetOutputCurrentType(
	DWORD           dwOutputStreamID,
	IMFMediaType    **ppType)
{
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
	return S_OK;
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
	DWORD bufferCount = 0;
	auto hr = pSample->GetBufferCount(&bufferCount);
	ARC_ThrowIfFailed(hr);

	// buffer count is almost always one, that is why
	// I'm optimizing this particular case and not bothering to copy the buffers
	if (bufferCount == 1)
	{
		ComPtr<IMFMediaBuffer> inputMediaBuffer = nullptr;
		hr = pSample->GetBufferByIndex(0, &inputMediaBuffer);
		ARC_ThrowIfFailed(hr);

		BYTE* inBuffer = nullptr;
		DWORD bufferLength = 0;
		hr = inputMediaBuffer->Lock(&inBuffer, NULL, &bufferLength);
		ARC_ThrowIfFailed(hr);
		Util::ScopeGuard<std::function<void()>> inputBufferUnlocker([&inputMediaBuffer]()
		{
			auto hr = inputMediaBuffer->Unlock();
			ARC_ASSERT(SUCCEEDED(hr));
		});

		BitBuffer bitsWrapper;
		BitBufferInit(&bitsWrapper, inBuffer, bufferLength);

		auto decodeResult = m_decoder.Decode(&bitsWrapper, (uint8_t*) (m_outBuffer.data()), m_alacBox->GetSamplePerFrame(), m_channelCount, &m_samplesAvailable);
		m_hasEnoughInput = decodeResult != kALAC_ParamError;
		m_isOutFrameReady = (decodeResult == 0);
	}
	else
	{
		// there are multiple buffers, however the decoder can only accept a single continuous buffer
		// so we'll have to merge them together into a single buffer
		std::vector<char> inputFrameBuffer;
		for (unsigned int i = 0; i < bufferCount; ++i)
		{
			ComPtr<IMFMediaBuffer> inputMediaBuffer = nullptr;
			hr = pSample->GetBufferByIndex(i, &inputMediaBuffer);
			ARC_ThrowIfFailed(hr);

			BYTE* inBuffer = nullptr;
			DWORD bufferLength = 0;
			hr = inputMediaBuffer->Lock(&inBuffer, NULL, &bufferLength);
			ARC_ThrowIfFailed(hr);
			Util::ScopeGuard<std::function<void()>> inputBufferUnlocker([&inputMediaBuffer]()
			{
				auto hr = inputMediaBuffer->Unlock();
				ARC_ASSERT(SUCCEEDED(hr));
			});

			auto nextPos = inputFrameBuffer.size();
			inputFrameBuffer.resize(inputFrameBuffer.size() + bufferLength);
			std::copy(	inBuffer,
						inBuffer + bufferLength,
						inputFrameBuffer.data() + nextPos);
		}

		BitBuffer bitsWrapper;
		BitBufferInit(&bitsWrapper, (uint8_t*) (inputFrameBuffer.data()), (uint32_t)(inputFrameBuffer.size()));

		auto decodeResult = m_decoder.Decode(&bitsWrapper, (uint8_t*) (m_outBuffer.data()), m_alacBox->GetSamplePerFrame(), m_channelCount, &m_samplesAvailable);
		m_hasEnoughInput = decodeResult != kALAC_ParamError;
		m_isOutFrameReady = decodeResult == 0;
	}

	return S_OK;
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
	if (m_isOutFrameReady)
	{
		ComPtr<IMFMediaBuffer> outBuffer = nullptr;
		auto hr = pOutputSamples->pSample->GetBufferByIndex(cOutputBufferCount - 1, &outBuffer);
		ARC_ThrowIfFailed(hr);

		DWORD bufferLength = 0;
		DWORD currentLength = 0;

		BYTE* pBuffer = nullptr;
		hr = outBuffer->Lock(&pBuffer, &bufferLength, &currentLength);
		ARC_ThrowIfFailed(hr);

		Util::ScopeGuard<std::function<void()>> inputBufferUnlocker([&outBuffer]()
		{
			auto hr = outBuffer->Unlock();
			ARC_ASSERT(SUCCEEDED(hr));
		});

		ARC_ASSERT(bufferLength >= m_outBuffer.size());
		auto bytesToCopy =  m_samplesAvailable * m_channelCount * m_bitsPerSample / 8;
		ARC_ASSERT(bytesToCopy <= m_outBuffer.size());

		std::copy(	m_outBuffer.data(), 
					m_outBuffer.data() + bytesToCopy,
					pBuffer);

		outBuffer->SetCurrentLength(bytesToCopy);
		m_isOutFrameReady = false;
		m_samplesAvailable = 0;

		return S_OK;
	}
	else
	{
		return MF_E_TRANSFORM_NEED_MORE_INPUT;
	}
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