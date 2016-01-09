#include "NativeBufferWrapper.hpp"

#include "CheckedCasts.hpp"

namespace FileSystem {

NativeBufferWrapper::~NativeBufferWrapper() {}

STDMETHODIMP NativeBufferWrapper::RuntimeClassInitialize(std::vector<unsigned char>* buffer) {
  m_buffer = buffer;
  return S_OK;
}

STDMETHODIMP NativeBufferWrapper::Buffer(byte** value) {
  *value = &(*m_buffer)[0];
  return S_OK;
}

STDMETHODIMP NativeBufferWrapper::get_Capacity(UINT32* value) {
  *value = Util::SafeIntCast<UINT32>(m_buffer->capacity());
  return S_OK;
}

STDMETHODIMP NativeBufferWrapper::get_Length(UINT32* value) {
  *value = Util::SafeIntCast<UINT32>(m_buffer->size());
  return S_OK;
}

STDMETHODIMP NativeBufferWrapper::put_Length(UINT32 value) {
  m_buffer->resize(value);
  return S_OK;
}

Windows::Storage::Streams::IBuffer ^ NativeBufferWrapper::WrapBuffer(std::vector<unsigned char>* wrappedBuffer) {
  using namespace Microsoft::WRL;
  ComPtr<NativeBufferWrapper> nativeBuffer;
  MakeAndInitialize<NativeBufferWrapper>(&nativeBuffer, wrappedBuffer);
  auto iinspectable = (IInspectable*)reinterpret_cast<IInspectable*>(nativeBuffer.Get());
  Windows::Storage::Streams::IBuffer ^ buffer = reinterpret_cast<Windows::Storage::Streams::IBuffer ^>(iinspectable);
  return buffer;
}
}