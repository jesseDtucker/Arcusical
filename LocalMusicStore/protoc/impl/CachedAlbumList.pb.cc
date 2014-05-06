// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: CachedAlbumList.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "CachedAlbumList.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace Arcusical {
namespace LocalMusicStore {

namespace {

const ::google::protobuf::Descriptor* CachedAlbumList_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  CachedAlbumList_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_CachedAlbumList_2eproto() {
  protobuf_AddDesc_CachedAlbumList_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "CachedAlbumList.proto");
  GOOGLE_CHECK(file != NULL);
  CachedAlbumList_descriptor_ = file->message_type(0);
  static const int CachedAlbumList_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CachedAlbumList, albums_),
  };
  CachedAlbumList_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      CachedAlbumList_descriptor_,
      CachedAlbumList::default_instance_,
      CachedAlbumList_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CachedAlbumList, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CachedAlbumList, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(CachedAlbumList));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_CachedAlbumList_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    CachedAlbumList_descriptor_, &CachedAlbumList::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_CachedAlbumList_2eproto() {
  delete CachedAlbumList::default_instance_;
  delete CachedAlbumList_reflection_;
}

void protobuf_AddDesc_CachedAlbumList_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::Arcusical::LocalMusicStore::protobuf_AddDesc_CachedAlbum_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\025CachedAlbumList.proto\022\031Arcusical.Local"
    "MusicStore\032\021CachedAlbum.proto\"I\n\017CachedA"
    "lbumList\0226\n\006albums\030\001 \003(\0132&.Arcusical.Loc"
    "alMusicStore.CachedAlbum", 144);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "CachedAlbumList.proto", &protobuf_RegisterTypes);
  CachedAlbumList::default_instance_ = new CachedAlbumList();
  CachedAlbumList::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_CachedAlbumList_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_CachedAlbumList_2eproto {
  StaticDescriptorInitializer_CachedAlbumList_2eproto() {
    protobuf_AddDesc_CachedAlbumList_2eproto();
  }
} static_descriptor_initializer_CachedAlbumList_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int CachedAlbumList::kAlbumsFieldNumber;
#endif  // !_MSC_VER

CachedAlbumList::CachedAlbumList()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void CachedAlbumList::InitAsDefaultInstance() {
}

CachedAlbumList::CachedAlbumList(const CachedAlbumList& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void CachedAlbumList::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

CachedAlbumList::~CachedAlbumList() {
  SharedDtor();
}

void CachedAlbumList::SharedDtor() {
  if (this != default_instance_) {
  }
}

void CachedAlbumList::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* CachedAlbumList::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return CachedAlbumList_descriptor_;
}

const CachedAlbumList& CachedAlbumList::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_CachedAlbumList_2eproto();
  return *default_instance_;
}

CachedAlbumList* CachedAlbumList::default_instance_ = NULL;

CachedAlbumList* CachedAlbumList::New() const {
  return new CachedAlbumList;
}

void CachedAlbumList::Clear() {
  albums_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool CachedAlbumList::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated .Arcusical.LocalMusicStore.CachedAlbum albums = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_albums:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_albums()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(10)) goto parse_albums;
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void CachedAlbumList::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated .Arcusical.LocalMusicStore.CachedAlbum albums = 1;
  for (int i = 0; i < this->albums_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, this->albums(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* CachedAlbumList::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated .Arcusical.LocalMusicStore.CachedAlbum albums = 1;
  for (int i = 0; i < this->albums_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        1, this->albums(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int CachedAlbumList::ByteSize() const {
  int total_size = 0;

  // repeated .Arcusical.LocalMusicStore.CachedAlbum albums = 1;
  total_size += 1 * this->albums_size();
  for (int i = 0; i < this->albums_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->albums(i));
  }

  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void CachedAlbumList::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const CachedAlbumList* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const CachedAlbumList*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void CachedAlbumList::MergeFrom(const CachedAlbumList& from) {
  GOOGLE_CHECK_NE(&from, this);
  albums_.MergeFrom(from.albums_);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void CachedAlbumList::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void CachedAlbumList::CopyFrom(const CachedAlbumList& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CachedAlbumList::IsInitialized() const {

  for (int i = 0; i < albums_size(); i++) {
    if (!this->albums(i).IsInitialized()) return false;
  }
  return true;
}

void CachedAlbumList::Swap(CachedAlbumList* other) {
  if (other != this) {
    albums_.Swap(&other->albums_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata CachedAlbumList::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = CachedAlbumList_descriptor_;
  metadata.reflection = CachedAlbumList_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace LocalMusicStore
}  // namespace Arcusical

// @@protoc_insertion_point(global_scope)
