// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Echo.proto

#ifndef PROTOBUF_Echo_2eproto__INCLUDED
#define PROTOBUF_Echo_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3000000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3000000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace msg {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_Echo_2eproto();
void protobuf_AssignDesc_Echo_2eproto();
void protobuf_ShutdownFile_Echo_2eproto();

class Echo;

// ===================================================================

class Echo : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:msg.Echo) */ {
 public:
  Echo();
  virtual ~Echo();

  Echo(const Echo& from);

  inline Echo& operator=(const Echo& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Echo& default_instance();

  void Swap(Echo* other);

  // implements Message ----------------------------------------------

  inline Echo* New() const { return New(NULL); }

  Echo* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Echo& from);
  void MergeFrom(const Echo& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(Echo* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required string message_name = 1;
  bool has_message_name() const;
  void clear_message_name();
  static const int kMessageNameFieldNumber = 1;
  const ::std::string& message_name() const;
  void set_message_name(const ::std::string& value);
  void set_message_name(const char* value);
  void set_message_name(const char* value, size_t size);
  ::std::string* mutable_message_name();
  ::std::string* release_message_name();
  void set_allocated_message_name(::std::string* message_name);

  // required string content = 2;
  bool has_content() const;
  void clear_content();
  static const int kContentFieldNumber = 2;
  const ::std::string& content() const;
  void set_content(const ::std::string& value);
  void set_content(const char* value);
  void set_content(const char* value, size_t size);
  ::std::string* mutable_content();
  ::std::string* release_content();
  void set_allocated_content(::std::string* content);

  // @@protoc_insertion_point(class_scope:msg.Echo)
 private:
  inline void set_has_message_name();
  inline void clear_has_message_name();
  inline void set_has_content();
  inline void clear_has_content();

  // helper for ByteSize()
  int RequiredFieldsByteSizeFallback() const;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::internal::ArenaStringPtr message_name_;
  ::google::protobuf::internal::ArenaStringPtr content_;
  friend void  protobuf_AddDesc_Echo_2eproto();
  friend void protobuf_AssignDesc_Echo_2eproto();
  friend void protobuf_ShutdownFile_Echo_2eproto();

  void InitAsDefaultInstance();
  static Echo* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// Echo

// required string message_name = 1;
inline bool Echo::has_message_name() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Echo::set_has_message_name() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Echo::clear_has_message_name() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Echo::clear_message_name() {
  message_name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_message_name();
}
inline const ::std::string& Echo::message_name() const {
  // @@protoc_insertion_point(field_get:msg.Echo.message_name)
  return message_name_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Echo::set_message_name(const ::std::string& value) {
  set_has_message_name();
  message_name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:msg.Echo.message_name)
}
inline void Echo::set_message_name(const char* value) {
  set_has_message_name();
  message_name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:msg.Echo.message_name)
}
inline void Echo::set_message_name(const char* value, size_t size) {
  set_has_message_name();
  message_name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:msg.Echo.message_name)
}
inline ::std::string* Echo::mutable_message_name() {
  set_has_message_name();
  // @@protoc_insertion_point(field_mutable:msg.Echo.message_name)
  return message_name_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* Echo::release_message_name() {
  // @@protoc_insertion_point(field_release:msg.Echo.message_name)
  clear_has_message_name();
  return message_name_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Echo::set_allocated_message_name(::std::string* message_name) {
  if (message_name != NULL) {
    set_has_message_name();
  } else {
    clear_has_message_name();
  }
  message_name_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), message_name);
  // @@protoc_insertion_point(field_set_allocated:msg.Echo.message_name)
}

// required string content = 2;
inline bool Echo::has_content() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Echo::set_has_content() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Echo::clear_has_content() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Echo::clear_content() {
  content_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_content();
}
inline const ::std::string& Echo::content() const {
  // @@protoc_insertion_point(field_get:msg.Echo.content)
  return content_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Echo::set_content(const ::std::string& value) {
  set_has_content();
  content_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:msg.Echo.content)
}
inline void Echo::set_content(const char* value) {
  set_has_content();
  content_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:msg.Echo.content)
}
inline void Echo::set_content(const char* value, size_t size) {
  set_has_content();
  content_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:msg.Echo.content)
}
inline ::std::string* Echo::mutable_content() {
  set_has_content();
  // @@protoc_insertion_point(field_mutable:msg.Echo.content)
  return content_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* Echo::release_content() {
  // @@protoc_insertion_point(field_release:msg.Echo.content)
  clear_has_content();
  return content_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Echo::set_allocated_content(::std::string* content) {
  if (content != NULL) {
    set_has_content();
  } else {
    clear_has_content();
  }
  content_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), content);
  // @@protoc_insertion_point(field_set_allocated:msg.Echo.content)
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace msg

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_Echo_2eproto__INCLUDED