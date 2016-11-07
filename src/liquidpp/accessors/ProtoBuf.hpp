#pragma once

#include "../Accessor.hpp"

/*
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/reflection.h>
*/

namespace google
{
   namespace protobuf
   {
      class Message;
   }
}

namespace liquidpp {

template <> struct Accessor<google::protobuf::Message> : public std::true_type {
  template<typename T, typename FieldDescriptor>
  static Value getRepeated(const T& msg,
                           const FieldDescriptor* field,
                           PathRef path) {
    auto refl = msg.GetReflection();
    auto cnt = refl->FieldSize(msg, field);
    auto key = popKey(path);
    if (!key)
      return RangeDefinition{static_cast<size_t>(cnt)};
    
    if (key.isName())
      return ValueTag::SubValue;
    auto idx = static_cast<int>(key.index());
    if (idx >= cnt)
      return ValueTag::OutOfRange;
    
    switch (field->cpp_type()) {
    case FieldDescriptor::CPPTYPE_INT32:
       return elementToValue(refl->GetRepeatedInt32(msg, field, idx), path);
    case FieldDescriptor::CPPTYPE_INT64:
       return elementToValue(refl->GetRepeatedInt64(msg, field, idx), path);
    case FieldDescriptor::CPPTYPE_UINT32:
       return elementToValue(refl->GetRepeatedUInt32(msg, field, idx), path);
    case FieldDescriptor::CPPTYPE_UINT64:
       return elementToValue(refl->GetRepeatedUInt64(msg, field, idx), path);
    case FieldDescriptor::CPPTYPE_DOUBLE:
       return elementToValue(refl->GetRepeatedDouble(msg, field, idx), path);
    case FieldDescriptor::CPPTYPE_FLOAT:
       return elementToValue(refl->GetRepeatedFloat(msg, field, idx), path);
    case FieldDescriptor::CPPTYPE_BOOL:
       return elementToValue(refl->GetRepeatedBool(msg, field, idx), path);
    case FieldDescriptor::CPPTYPE_ENUM:
       return elementToValue(refl->GetRepeatedEnum(msg, field, idx)->name(),
                            path);
    case FieldDescriptor::CPPTYPE_STRING:
       return elementToValue(refl->GetRepeatedString(msg, field, idx), path);
    case FieldDescriptor::CPPTYPE_MESSAGE:
       return elementToValue(refl->GetRepeatedMessage(msg, field, idx), path);
    }

    assert(false); // Unhandled field type
    return ValueTag::Null;
  }

  template<typename T>
  static Value get(const T& msg, PathRef &path) {
    auto key = popKey(path);
    if (!key)
      return ValueTag::Object;

    auto desc = msg.GetDescriptor();
    auto field = desc->FindFieldByName(key.name().to_string());
    if (!field)
      return ValueTag::Null;

    if (field->is_repeated())
      return getRepeated(msg, field, path);

    auto refl = msg.GetReflection();
    
    // with proto3 the default value is indiscernible from not set values
    //if (!refl->HasField(msg, field))
    //  return ValueTag::Null;

    using FieldDescriptor = std::decay_t<decltype(*field)>;
    switch (field->cpp_type()) {
    case FieldDescriptor::CPPTYPE_INT32:
       return elementToValue(refl->GetInt32(msg, field), path);
    case FieldDescriptor::CPPTYPE_INT64:
       return elementToValue(refl->GetInt64(msg, field), path);
    case FieldDescriptor::CPPTYPE_UINT32:
       return elementToValue(refl->GetUInt32(msg, field), path);
    case FieldDescriptor::CPPTYPE_UINT64:
       return elementToValue(refl->GetUInt64(msg, field), path);
    case FieldDescriptor::CPPTYPE_DOUBLE:
       return elementToValue(refl->GetDouble(msg, field), path);
    case FieldDescriptor::CPPTYPE_FLOAT:
       return elementToValue(refl->GetFloat(msg, field), path);
    case FieldDescriptor::CPPTYPE_BOOL:
       return elementToValue(refl->GetBool(msg, field), path);
    case FieldDescriptor::CPPTYPE_ENUM:
       return elementToValue(refl->GetEnum(msg, field)->name(), path);
    case FieldDescriptor::CPPTYPE_STRING:
       return elementToValue(refl->GetString(msg, field), path);
    case FieldDescriptor::CPPTYPE_MESSAGE:
       return elementToValue(refl->GetMessage(msg, field), path);
    }

    assert(false); // Unhandled field type
    return ValueTag::Null;
  }
};

template <typename T>
struct Accessor<
    T, std::enable_if_t<std::is_base_of<google::protobuf::Message, T>::value>>
    : public Accessor<google::protobuf::Message> {};
}
