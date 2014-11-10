// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: CachedSong.proto

#ifndef PROTOBUF_CachedSong_2eproto__INCLUDED
#define PROTOBUF_CachedSong_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
#include "GUID.pb.h"
// @@protoc_insertion_point(includes)

namespace Arcusical {
namespace LocalMusicStore {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_CachedSong_2eproto();
void protobuf_AssignDesc_CachedSong_2eproto();
void protobuf_ShutdownFile_CachedSong_2eproto();

class SongFile;
class CachedSong;

enum Encoding {
  UNKNOWN_ENCODING = 0,
  AAC = 1,
  ALAC = 2,
  MP3 = 3,
  FLAC = 4
};
bool Encoding_IsValid(int value);
const Encoding Encoding_MIN = UNKNOWN_ENCODING;
const Encoding Encoding_MAX = FLAC;
const int Encoding_ARRAYSIZE = Encoding_MAX + 1;

const ::google::protobuf::EnumDescriptor* Encoding_descriptor();
inline const ::std::string& Encoding_Name(Encoding value) {
  return ::google::protobuf::internal::NameOfEnum(
    Encoding_descriptor(), value);
}
inline bool Encoding_Parse(
    const ::std::string& name, Encoding* value) {
  return ::google::protobuf::internal::ParseNamedEnum<Encoding>(
    Encoding_descriptor(), name, value);
}
enum Container {
  UNKNOWN_CONTAINER = 0,
  MP3_CONTAINER = 1,
  MPEG4_CONTAINER = 2,
  FLAC_CONTAINER = 3,
  WAV_CONTAINER = 4
};
bool Container_IsValid(int value);
const Container Container_MIN = UNKNOWN_CONTAINER;
const Container Container_MAX = WAV_CONTAINER;
const int Container_ARRAYSIZE = Container_MAX + 1;

const ::google::protobuf::EnumDescriptor* Container_descriptor();
inline const ::std::string& Container_Name(Container value) {
  return ::google::protobuf::internal::NameOfEnum(
    Container_descriptor(), value);
}
inline bool Container_Parse(
    const ::std::string& name, Container* value) {
  return ::google::protobuf::internal::ParseNamedEnum<Container>(
    Container_descriptor(), name, value);
}
// ===================================================================

class SongFile : public ::google::protobuf::Message {
 public:
  SongFile();
  virtual ~SongFile();

  SongFile(const SongFile& from);

  inline SongFile& operator=(const SongFile& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const SongFile& default_instance();

  void Swap(SongFile* other);

  // implements Message ----------------------------------------------

  SongFile* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const SongFile& from);
  void MergeFrom(const SongFile& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required .Arcusical.LocalMusicStore.Encoding encoding = 1;
  inline bool has_encoding() const;
  inline void clear_encoding();
  static const int kEncodingFieldNumber = 1;
  inline ::Arcusical::LocalMusicStore::Encoding encoding() const;
  inline void set_encoding(::Arcusical::LocalMusicStore::Encoding value);

  // required string file = 2;
  inline bool has_file() const;
  inline void clear_file();
  static const int kFileFieldNumber = 2;
  inline const ::std::string& file() const;
  inline void set_file(const ::std::string& value);
  inline void set_file(const char* value);
  inline void set_file(const char* value, size_t size);
  inline ::std::string* mutable_file();
  inline ::std::string* release_file();
  inline void set_allocated_file(::std::string* file);

  // required int32 bitRate = 3;
  inline bool has_bitrate() const;
  inline void clear_bitrate();
  static const int kBitRateFieldNumber = 3;
  inline ::google::protobuf::int32 bitrate() const;
  inline void set_bitrate(::google::protobuf::int32 value);

  // required int32 sampleSize = 4;
  inline bool has_samplesize() const;
  inline void clear_samplesize();
  static const int kSampleSizeFieldNumber = 4;
  inline ::google::protobuf::int32 samplesize() const;
  inline void set_samplesize(::google::protobuf::int32 value);

  // required int32 channelCount = 5;
  inline bool has_channelcount() const;
  inline void clear_channelcount();
  static const int kChannelCountFieldNumber = 5;
  inline ::google::protobuf::int32 channelcount() const;
  inline void set_channelcount(::google::protobuf::int32 value);

  // required .Arcusical.LocalMusicStore.Container container = 6;
  inline bool has_container() const;
  inline void clear_container();
  static const int kContainerFieldNumber = 6;
  inline ::Arcusical::LocalMusicStore::Container container() const;
  inline void set_container(::Arcusical::LocalMusicStore::Container value);

  // @@protoc_insertion_point(class_scope:Arcusical.LocalMusicStore.SongFile)
 private:
  inline void set_has_encoding();
  inline void clear_has_encoding();
  inline void set_has_file();
  inline void clear_has_file();
  inline void set_has_bitrate();
  inline void clear_has_bitrate();
  inline void set_has_samplesize();
  inline void clear_has_samplesize();
  inline void set_has_channelcount();
  inline void clear_has_channelcount();
  inline void set_has_container();
  inline void clear_has_container();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* file_;
  int encoding_;
  ::google::protobuf::int32 bitrate_;
  ::google::protobuf::int32 samplesize_;
  ::google::protobuf::int32 channelcount_;
  int container_;
  friend void  protobuf_AddDesc_CachedSong_2eproto();
  friend void protobuf_AssignDesc_CachedSong_2eproto();
  friend void protobuf_ShutdownFile_CachedSong_2eproto();

  void InitAsDefaultInstance();
  static SongFile* default_instance_;
};
// -------------------------------------------------------------------

class CachedSong : public ::google::protobuf::Message {
 public:
  CachedSong();
  virtual ~CachedSong();

  CachedSong(const CachedSong& from);

  inline CachedSong& operator=(const CachedSong& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const CachedSong& default_instance();

  void Swap(CachedSong* other);

  // implements Message ----------------------------------------------

  CachedSong* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const CachedSong& from);
  void MergeFrom(const CachedSong& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required .Arcusical.LocalMusicStore.GUID id = 1;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 1;
  inline const ::Arcusical::LocalMusicStore::GUID& id() const;
  inline ::Arcusical::LocalMusicStore::GUID* mutable_id();
  inline ::Arcusical::LocalMusicStore::GUID* release_id();
  inline void set_allocated_id(::Arcusical::LocalMusicStore::GUID* id);

  // required string title = 2;
  inline bool has_title() const;
  inline void clear_title();
  static const int kTitleFieldNumber = 2;
  inline const ::std::string& title() const;
  inline void set_title(const ::std::string& value);
  inline void set_title(const char* value);
  inline void set_title(const char* value, size_t size);
  inline ::std::string* mutable_title();
  inline ::std::string* release_title();
  inline void set_allocated_title(::std::string* title);

  // required string artist = 4;
  inline bool has_artist() const;
  inline void clear_artist();
  static const int kArtistFieldNumber = 4;
  inline const ::std::string& artist() const;
  inline void set_artist(const ::std::string& value);
  inline void set_artist(const char* value);
  inline void set_artist(const char* value, size_t size);
  inline ::std::string* mutable_artist();
  inline ::std::string* release_artist();
  inline void set_allocated_artist(::std::string* artist);

  // required int64 length = 5;
  inline bool has_length() const;
  inline void clear_length();
  static const int kLengthFieldNumber = 5;
  inline ::google::protobuf::int64 length() const;
  inline void set_length(::google::protobuf::int64 value);

  // repeated .Arcusical.LocalMusicStore.SongFile files = 6;
  inline int files_size() const;
  inline void clear_files();
  static const int kFilesFieldNumber = 6;
  inline const ::Arcusical::LocalMusicStore::SongFile& files(int index) const;
  inline ::Arcusical::LocalMusicStore::SongFile* mutable_files(int index);
  inline ::Arcusical::LocalMusicStore::SongFile* add_files();
  inline const ::google::protobuf::RepeatedPtrField< ::Arcusical::LocalMusicStore::SongFile >&
      files() const;
  inline ::google::protobuf::RepeatedPtrField< ::Arcusical::LocalMusicStore::SongFile >*
      mutable_files();

  // @@protoc_insertion_point(class_scope:Arcusical.LocalMusicStore.CachedSong)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_title();
  inline void clear_has_title();
  inline void set_has_artist();
  inline void clear_has_artist();
  inline void set_has_length();
  inline void clear_has_length();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::Arcusical::LocalMusicStore::GUID* id_;
  ::std::string* title_;
  ::std::string* artist_;
  ::google::protobuf::int64 length_;
  ::google::protobuf::RepeatedPtrField< ::Arcusical::LocalMusicStore::SongFile > files_;
  friend void  protobuf_AddDesc_CachedSong_2eproto();
  friend void protobuf_AssignDesc_CachedSong_2eproto();
  friend void protobuf_ShutdownFile_CachedSong_2eproto();

  void InitAsDefaultInstance();
  static CachedSong* default_instance_;
};
// ===================================================================


// ===================================================================

// SongFile

// required .Arcusical.LocalMusicStore.Encoding encoding = 1;
inline bool SongFile::has_encoding() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void SongFile::set_has_encoding() {
  _has_bits_[0] |= 0x00000001u;
}
inline void SongFile::clear_has_encoding() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void SongFile::clear_encoding() {
  encoding_ = 0;
  clear_has_encoding();
}
inline ::Arcusical::LocalMusicStore::Encoding SongFile::encoding() const {
  // @@protoc_insertion_point(field_get:Arcusical.LocalMusicStore.SongFile.encoding)
  return static_cast< ::Arcusical::LocalMusicStore::Encoding >(encoding_);
}
inline void SongFile::set_encoding(::Arcusical::LocalMusicStore::Encoding value) {
  assert(::Arcusical::LocalMusicStore::Encoding_IsValid(value));
  set_has_encoding();
  encoding_ = value;
  // @@protoc_insertion_point(field_set:Arcusical.LocalMusicStore.SongFile.encoding)
}

// required string file = 2;
inline bool SongFile::has_file() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void SongFile::set_has_file() {
  _has_bits_[0] |= 0x00000002u;
}
inline void SongFile::clear_has_file() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void SongFile::clear_file() {
  if (file_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_->clear();
  }
  clear_has_file();
}
inline const ::std::string& SongFile::file() const {
  // @@protoc_insertion_point(field_get:Arcusical.LocalMusicStore.SongFile.file)
  return *file_;
}
inline void SongFile::set_file(const ::std::string& value) {
  set_has_file();
  if (file_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_ = new ::std::string;
  }
  file_->assign(value);
  // @@protoc_insertion_point(field_set:Arcusical.LocalMusicStore.SongFile.file)
}
inline void SongFile::set_file(const char* value) {
  set_has_file();
  if (file_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_ = new ::std::string;
  }
  file_->assign(value);
  // @@protoc_insertion_point(field_set_char:Arcusical.LocalMusicStore.SongFile.file)
}
inline void SongFile::set_file(const char* value, size_t size) {
  set_has_file();
  if (file_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_ = new ::std::string;
  }
  file_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:Arcusical.LocalMusicStore.SongFile.file)
}
inline ::std::string* SongFile::mutable_file() {
  set_has_file();
  if (file_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    file_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:Arcusical.LocalMusicStore.SongFile.file)
  return file_;
}
inline ::std::string* SongFile::release_file() {
  clear_has_file();
  if (file_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = file_;
    file_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void SongFile::set_allocated_file(::std::string* file) {
  if (file_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete file_;
  }
  if (file) {
    set_has_file();
    file_ = file;
  } else {
    clear_has_file();
    file_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:Arcusical.LocalMusicStore.SongFile.file)
}

// required int32 bitRate = 3;
inline bool SongFile::has_bitrate() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void SongFile::set_has_bitrate() {
  _has_bits_[0] |= 0x00000004u;
}
inline void SongFile::clear_has_bitrate() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void SongFile::clear_bitrate() {
  bitrate_ = 0;
  clear_has_bitrate();
}
inline ::google::protobuf::int32 SongFile::bitrate() const {
  // @@protoc_insertion_point(field_get:Arcusical.LocalMusicStore.SongFile.bitRate)
  return bitrate_;
}
inline void SongFile::set_bitrate(::google::protobuf::int32 value) {
  set_has_bitrate();
  bitrate_ = value;
  // @@protoc_insertion_point(field_set:Arcusical.LocalMusicStore.SongFile.bitRate)
}

// required int32 sampleSize = 4;
inline bool SongFile::has_samplesize() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void SongFile::set_has_samplesize() {
  _has_bits_[0] |= 0x00000008u;
}
inline void SongFile::clear_has_samplesize() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void SongFile::clear_samplesize() {
  samplesize_ = 0;
  clear_has_samplesize();
}
inline ::google::protobuf::int32 SongFile::samplesize() const {
  // @@protoc_insertion_point(field_get:Arcusical.LocalMusicStore.SongFile.sampleSize)
  return samplesize_;
}
inline void SongFile::set_samplesize(::google::protobuf::int32 value) {
  set_has_samplesize();
  samplesize_ = value;
  // @@protoc_insertion_point(field_set:Arcusical.LocalMusicStore.SongFile.sampleSize)
}

// required int32 channelCount = 5;
inline bool SongFile::has_channelcount() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void SongFile::set_has_channelcount() {
  _has_bits_[0] |= 0x00000010u;
}
inline void SongFile::clear_has_channelcount() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void SongFile::clear_channelcount() {
  channelcount_ = 0;
  clear_has_channelcount();
}
inline ::google::protobuf::int32 SongFile::channelcount() const {
  // @@protoc_insertion_point(field_get:Arcusical.LocalMusicStore.SongFile.channelCount)
  return channelcount_;
}
inline void SongFile::set_channelcount(::google::protobuf::int32 value) {
  set_has_channelcount();
  channelcount_ = value;
  // @@protoc_insertion_point(field_set:Arcusical.LocalMusicStore.SongFile.channelCount)
}

// required .Arcusical.LocalMusicStore.Container container = 6;
inline bool SongFile::has_container() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void SongFile::set_has_container() {
  _has_bits_[0] |= 0x00000020u;
}
inline void SongFile::clear_has_container() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void SongFile::clear_container() {
  container_ = 0;
  clear_has_container();
}
inline ::Arcusical::LocalMusicStore::Container SongFile::container() const {
  // @@protoc_insertion_point(field_get:Arcusical.LocalMusicStore.SongFile.container)
  return static_cast< ::Arcusical::LocalMusicStore::Container >(container_);
}
inline void SongFile::set_container(::Arcusical::LocalMusicStore::Container value) {
  assert(::Arcusical::LocalMusicStore::Container_IsValid(value));
  set_has_container();
  container_ = value;
  // @@protoc_insertion_point(field_set:Arcusical.LocalMusicStore.SongFile.container)
}

// -------------------------------------------------------------------

// CachedSong

// required .Arcusical.LocalMusicStore.GUID id = 1;
inline bool CachedSong::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void CachedSong::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void CachedSong::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void CachedSong::clear_id() {
  if (id_ != NULL) id_->::Arcusical::LocalMusicStore::GUID::Clear();
  clear_has_id();
}
inline const ::Arcusical::LocalMusicStore::GUID& CachedSong::id() const {
  // @@protoc_insertion_point(field_get:Arcusical.LocalMusicStore.CachedSong.id)
  return id_ != NULL ? *id_ : *default_instance_->id_;
}
inline ::Arcusical::LocalMusicStore::GUID* CachedSong::mutable_id() {
  set_has_id();
  if (id_ == NULL) id_ = new ::Arcusical::LocalMusicStore::GUID;
  // @@protoc_insertion_point(field_mutable:Arcusical.LocalMusicStore.CachedSong.id)
  return id_;
}
inline ::Arcusical::LocalMusicStore::GUID* CachedSong::release_id() {
  clear_has_id();
  ::Arcusical::LocalMusicStore::GUID* temp = id_;
  id_ = NULL;
  return temp;
}
inline void CachedSong::set_allocated_id(::Arcusical::LocalMusicStore::GUID* id) {
  delete id_;
  id_ = id;
  if (id) {
    set_has_id();
  } else {
    clear_has_id();
  }
  // @@protoc_insertion_point(field_set_allocated:Arcusical.LocalMusicStore.CachedSong.id)
}

// required string title = 2;
inline bool CachedSong::has_title() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void CachedSong::set_has_title() {
  _has_bits_[0] |= 0x00000002u;
}
inline void CachedSong::clear_has_title() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void CachedSong::clear_title() {
  if (title_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    title_->clear();
  }
  clear_has_title();
}
inline const ::std::string& CachedSong::title() const {
  // @@protoc_insertion_point(field_get:Arcusical.LocalMusicStore.CachedSong.title)
  return *title_;
}
inline void CachedSong::set_title(const ::std::string& value) {
  set_has_title();
  if (title_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    title_ = new ::std::string;
  }
  title_->assign(value);
  // @@protoc_insertion_point(field_set:Arcusical.LocalMusicStore.CachedSong.title)
}
inline void CachedSong::set_title(const char* value) {
  set_has_title();
  if (title_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    title_ = new ::std::string;
  }
  title_->assign(value);
  // @@protoc_insertion_point(field_set_char:Arcusical.LocalMusicStore.CachedSong.title)
}
inline void CachedSong::set_title(const char* value, size_t size) {
  set_has_title();
  if (title_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    title_ = new ::std::string;
  }
  title_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:Arcusical.LocalMusicStore.CachedSong.title)
}
inline ::std::string* CachedSong::mutable_title() {
  set_has_title();
  if (title_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    title_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:Arcusical.LocalMusicStore.CachedSong.title)
  return title_;
}
inline ::std::string* CachedSong::release_title() {
  clear_has_title();
  if (title_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = title_;
    title_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void CachedSong::set_allocated_title(::std::string* title) {
  if (title_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete title_;
  }
  if (title) {
    set_has_title();
    title_ = title;
  } else {
    clear_has_title();
    title_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:Arcusical.LocalMusicStore.CachedSong.title)
}

// required string artist = 4;
inline bool CachedSong::has_artist() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void CachedSong::set_has_artist() {
  _has_bits_[0] |= 0x00000004u;
}
inline void CachedSong::clear_has_artist() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void CachedSong::clear_artist() {
  if (artist_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    artist_->clear();
  }
  clear_has_artist();
}
inline const ::std::string& CachedSong::artist() const {
  // @@protoc_insertion_point(field_get:Arcusical.LocalMusicStore.CachedSong.artist)
  return *artist_;
}
inline void CachedSong::set_artist(const ::std::string& value) {
  set_has_artist();
  if (artist_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    artist_ = new ::std::string;
  }
  artist_->assign(value);
  // @@protoc_insertion_point(field_set:Arcusical.LocalMusicStore.CachedSong.artist)
}
inline void CachedSong::set_artist(const char* value) {
  set_has_artist();
  if (artist_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    artist_ = new ::std::string;
  }
  artist_->assign(value);
  // @@protoc_insertion_point(field_set_char:Arcusical.LocalMusicStore.CachedSong.artist)
}
inline void CachedSong::set_artist(const char* value, size_t size) {
  set_has_artist();
  if (artist_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    artist_ = new ::std::string;
  }
  artist_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:Arcusical.LocalMusicStore.CachedSong.artist)
}
inline ::std::string* CachedSong::mutable_artist() {
  set_has_artist();
  if (artist_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    artist_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:Arcusical.LocalMusicStore.CachedSong.artist)
  return artist_;
}
inline ::std::string* CachedSong::release_artist() {
  clear_has_artist();
  if (artist_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = artist_;
    artist_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void CachedSong::set_allocated_artist(::std::string* artist) {
  if (artist_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete artist_;
  }
  if (artist) {
    set_has_artist();
    artist_ = artist;
  } else {
    clear_has_artist();
    artist_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:Arcusical.LocalMusicStore.CachedSong.artist)
}

// required int64 length = 5;
inline bool CachedSong::has_length() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void CachedSong::set_has_length() {
  _has_bits_[0] |= 0x00000008u;
}
inline void CachedSong::clear_has_length() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void CachedSong::clear_length() {
  length_ = GOOGLE_LONGLONG(0);
  clear_has_length();
}
inline ::google::protobuf::int64 CachedSong::length() const {
  // @@protoc_insertion_point(field_get:Arcusical.LocalMusicStore.CachedSong.length)
  return length_;
}
inline void CachedSong::set_length(::google::protobuf::int64 value) {
  set_has_length();
  length_ = value;
  // @@protoc_insertion_point(field_set:Arcusical.LocalMusicStore.CachedSong.length)
}

// repeated .Arcusical.LocalMusicStore.SongFile files = 6;
inline int CachedSong::files_size() const {
  return files_.size();
}
inline void CachedSong::clear_files() {
  files_.Clear();
}
inline const ::Arcusical::LocalMusicStore::SongFile& CachedSong::files(int index) const {
  // @@protoc_insertion_point(field_get:Arcusical.LocalMusicStore.CachedSong.files)
  return files_.Get(index);
}
inline ::Arcusical::LocalMusicStore::SongFile* CachedSong::mutable_files(int index) {
  // @@protoc_insertion_point(field_mutable:Arcusical.LocalMusicStore.CachedSong.files)
  return files_.Mutable(index);
}
inline ::Arcusical::LocalMusicStore::SongFile* CachedSong::add_files() {
  // @@protoc_insertion_point(field_add:Arcusical.LocalMusicStore.CachedSong.files)
  return files_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::Arcusical::LocalMusicStore::SongFile >&
CachedSong::files() const {
  // @@protoc_insertion_point(field_list:Arcusical.LocalMusicStore.CachedSong.files)
  return files_;
}
inline ::google::protobuf::RepeatedPtrField< ::Arcusical::LocalMusicStore::SongFile >*
CachedSong::mutable_files() {
  // @@protoc_insertion_point(field_mutable_list:Arcusical.LocalMusicStore.CachedSong.files)
  return &files_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace LocalMusicStore
}  // namespace Arcusical

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::Arcusical::LocalMusicStore::Encoding> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Arcusical::LocalMusicStore::Encoding>() {
  return ::Arcusical::LocalMusicStore::Encoding_descriptor();
}
template <> struct is_proto_enum< ::Arcusical::LocalMusicStore::Container> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Arcusical::LocalMusicStore::Container>() {
  return ::Arcusical::LocalMusicStore::Container_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_CachedSong_2eproto__INCLUDED
