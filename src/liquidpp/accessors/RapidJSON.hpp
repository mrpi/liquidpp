#pragma once

#include "../ValueConverter.hpp"
#include "../Key.hpp"

namespace rapidjson
{
template <typename Encoding, typename Allocator>
class GenericValue;

template <typename Encoding, typename Allocator, typename StackAllocator>
class GenericDocument;

template<typename CharType>
struct GenericStringRef;

template<typename CharType>
inline GenericStringRef<CharType> StringRef(const CharType* str, size_t length);
}

namespace liquidpp
{
template<typename Encoding, typename Allocator>
struct ValueConverter<rapidjson::GenericValue<Encoding, Allocator>> : public std::true_type
{
   template<typename T>
   static auto get(T&& parent)
   {
      return [&parent](OptIndex idx, string_view path) -> Value
      {
         using Value = rapidjson::GenericValue<Encoding, Allocator>;
         const Value* v = &parent;
         typename Value::ConstMemberIterator itr;

         while (auto key = popKey(path))
         {
            if (!v->IsObject())
               return ValueTag::Null;

            // Workarround: rapidjson 0.12 requires '\0'-termination even when size is given
            std::string keyName{key.name.data(), key.name.size()};
            itr = v->FindMember(rapidjson::StringRef(keyName.c_str(), keyName.size()));
            if (itr == v->MemberEnd())
               return ValueTag::Null;

            v = &itr->value;
            if (key.idx)
            {
               if (!v->IsArray())
                  return ValueTag::Null;
               if (*key.idx >= v->Size())
                  return ValueTag::OutOfRange;
               v = &(*v)[*key.idx];
            }
         }

         if (v->IsBool())
            return toValue(v->GetBool());
         if (v->IsNumber())
         {
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
            return std::string{v->GetString(), v->GetStringLength()};

         if (v->IsObject())
            return ValueTag::Object;
         if (v->IsArray())
            return ValueTag::Range;
         return ValueTag::Null;
      };
   }
};

template<typename Encoding, typename Allocator, typename StackAllocator>
struct ValueConverter<rapidjson::GenericDocument<Encoding, Allocator, StackAllocator>> : public std::true_type
{
   template<typename T>
   static auto get(T&& parent)
   {
      return ValueConverter<rapidjson::GenericValue<Encoding, Allocator>>::get(std::forward<T>(parent));
   }
};
}
