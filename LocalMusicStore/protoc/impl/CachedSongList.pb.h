// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: CachedSongList.proto

#ifndef PROTOBUF_CachedSongList_2eproto__INCLUDED
#define PROTOBUF_CachedSongList_2eproto__INCLUDED

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
#include <google/protobuf/unknown_field_set.h>
#include "CachedSong.pb.h"
// @@protoc_insertion_point(includes)

namespace Arcusical {
namespace LocalMusicStore {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_CachedSongList_2eproto();
void protobuf_AssignDesc_CachedSongList_2eproto();
void protobuf_ShutdownFile_CachedSongList_2eproto();

class CachedSongList;

// ===================================================================

class CachedSongList : public ::google::protobuf::Message {
 public:
  CachedSongList();
  virtual ~CachedSongList();

  CachedSongList(const CachedSongList& from);

  inline CachedSongList& operator=(const CachedSongList& from) {
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
  static const CachedSongList& default_instance();

  void Swap(CachedSongList* other);

  // implements Message ----------------------------------------------

  CachedSongList* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const CachedSongList& from);
  void MergeFrom(const CachedSongList& from);
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

  // repeated .Arcusical.LocalMusicStore.CachedSong songs = 1;
  inline int songs_size() const;
  inline void clear_songs();
  static const int kSongsFieldNumber = 1;
  inline const ::Arcusical::LocalMusicStore::CachedSong& songs(int index) const;
  inline ::Arcusical::LocalMusicStore::CachedSong* mutable_songs(int index);
  inline ::Arcusical::LocalMusicStore::CachedSong* add_songs();
  inline const ::google::protobuf::RepeatedPtrField< ::Arcusical::LocalMusicStore::CachedSong >&
      songs() const;
  inline ::google::protobuf::RepeatedPtrField< ::Arcusical::LocalMusicStore::CachedSong >*
      mutable_songs();

  // @@protoc_insertion_point(class_scope:Arcusical.LocalMusicStore.CachedSongList)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::RepeatedPtrField< ::Arcusical::LocalMusicStore::CachedSong > songs_;
  friend void  protobuf_AddDesc_CachedSongList_2eproto();
  friend void protobuf_AssignDesc_CachedSongList_2eproto();
  friend void protobuf_ShutdownFile_CachedSongList_2eproto();

  void InitAsDefaultInstance();
  static CachedSongList* default_instance_;
};
// ===================================================================


// ===================================================================

// CachedSongList

// repeated .Arcusical.LocalMusicStore.CachedSong songs = 1;
inline int CachedSongList::songs_size() const {
  return songs_.size();
}
inline void CachedSongList::clear_songs() {
  songs_.Clear();
}
inline const ::Arcusical::LocalMusicStore::CachedSong& CachedSongList::songs(int index) const {
  // @@protoc_insertion_point(field_get:Arcusical.LocalMusicStore.CachedSongList.songs)
  return songs_.Get(index);
}
inline ::Arcusical::LocalMusicStore::CachedSong* CachedSongList::mutable_songs(int index) {
  // @@protoc_insertion_point(field_mutable:Arcusical.LocalMusicStore.CachedSongList.songs)
  return songs_.Mutable(index);
}
inline ::Arcusical::LocalMusicStore::CachedSong* CachedSongList::add_songs() {
  // @@protoc_insertion_point(field_add:Arcusical.LocalMusicStore.CachedSongList.songs)
  return songs_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::Arcusical::LocalMusicStore::CachedSong >&
CachedSongList::songs() const {
  // @@protoc_insertion_point(field_list:Arcusical.LocalMusicStore.CachedSongList.songs)
  return songs_;
}
inline ::google::protobuf::RepeatedPtrField< ::Arcusical::LocalMusicStore::CachedSong >*
CachedSongList::mutable_songs() {
  // @@protoc_insertion_point(field_mutable_list:Arcusical.LocalMusicStore.CachedSongList.songs)
  return &songs_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace LocalMusicStore
}  // namespace Arcusical

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_CachedSongList_2eproto__INCLUDED
