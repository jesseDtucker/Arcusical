/*
*	Date: January 20th 2013
*	Author: Unknown
*	Description:
*	This is a winRT component that implements the IBuffer interface to allow for easy
*	reading of files from winRT APIs to a native format (vector<char> in this case).
*/
#pragma once
#ifndef NATIVE_BUFFER_HPP
#define NATIVE_BUFFER_HPP

#include <robuffer.h>
#include <windows.storage.streams.h>
#include <wrl.h>
#include <wrl/implements.h>
#include <vector>

namespace FileSystem {

class NativeBufferWrapper : public Microsoft::WRL::RuntimeClass<
                                Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>,
                                ABI::Windows::Storage::Streams::IBuffer, Windows::Storage::Streams::IBufferByteAccess> {
 public:
  virtual ~NativeBufferWrapper();

  STDMETHODIMP RuntimeClassInitialize(std::vector<unsigned char>* buffer);
  STDMETHODIMP Buffer(byte** value);
  STDMETHODIMP get_Capacity(UINT32* value);
  STDMETHODIMP get_Length(UINT32* value);
  STDMETHODIMP put_Length(UINT32 value);

  static Windows::Storage::Streams::IBuffer ^ WrapBuffer(std::vector<unsigned char>* buffer);

 private:
  std::vector<unsigned char>* m_buffer;
};
}

#endif