#pragma once

#include "../Key.hpp"
#include "../Accessor.hpp"

namespace rapidjson {
template <typename Encoding, typename Allocator> class GenericValue;

template <typename Encoding, typename Allocator, typename StackAllocator>
class GenericDocument;

template <typename CharType> struct GenericStringRef;

template <typename CharType>
inline GenericStringRef<CharType> StringRef(const CharType *str, size_t length);
}

namespace liquidpp {
template <typename Encoding, typename Allocator>
struct Accessor<rapidjson::GenericValue<Encoding, Allocator>>
    : public std::true_type {
  template <typename T>
  static Value get(const T& parent, PathRef path) {
      using Value = rapidjson::GenericValue<Encoding, Allocator>;
      const Value *v = &parent;
      typename Value::ConstMemberIterator itr;

      while (const auto key = popKey(path)) {
        if (key.isName()) {
          if (!v->IsObject())
            return ValueTag::SubValue;

          auto toRJString = [](auto str, size_t len) {
            return rapidjson::StringRef(str, len);
          };
          // Workarround: rapidjson 0.12 requires '\0'-termination even when
          // size is given
          auto &&name = key.name();
          auto start = name.data();
          auto end = start + name.size() + 1;
          SmallVector<char, 64> keyName(start, end);
          keyName.back() = '\0';
          itr = v->FindMember(toRJString(keyName.data(), keyName.size()));
          if (itr == v->MemberEnd())
            return ValueTag::Null;

          v = &itr->value;
        } else {
          if (!v->IsArray())
            return ValueTag::Null;
          if (key.index() >= v->Size())
            return ValueTag::OutOfRange;
          v = &(*v)[key.index()];
        }
      }

      if (v->IsBool())
        return toValue(v->GetBool());
      if (v->IsNumber()) {
        if (v->IsInt())
          return toValue(v->GetInt());
        if (v->IsUint())
          return toValue(v->GetUint());
        if (v->IsInt64())
          return toValue(v->GetInt64());
        if (v->IsUint64())
          return toValue(v->GetUint64());
        return toValue(v->GetDouble());
      }
      if (v->IsString())
        return liquidpp::Value::reference(
            {v->GetString(), v->GetStringLength()});

      if (v->IsObject())
        return ValueTag::Object;
      if (v->IsArray())
        return RangeDefinition{v->Size()};
      return ValueTag::Null;
  }
};

template <typename Encoding, typename Allocator, typename StackAllocator>
struct Accessor<
    rapidjson::GenericDocument<Encoding, Allocator, StackAllocator>>
    : public Accessor<rapidjson::GenericValue<Encoding, Allocator>> {
};

}
