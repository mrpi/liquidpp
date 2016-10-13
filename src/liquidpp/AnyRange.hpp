#pragma once

#include <type_traits>
#include <cassert>

namespace liquidpp
{
   template<typename ValueType>
   class AnyRange
   {
   public:
      virtual ~AnyRange()
      {}
      
      virtual bool hasNext() = 0
      
      virtual ValueType next() = 0;
   };
   
   template<typename T, typename ValueType = decltype(*std::declval<T>.begin())>
   class AnyRangeImpl : public AnyRange<ValueType>
   {
   private:
      T mContainerRef;
      boost::optional<decltype(std::declval<T>.begin())> mItr;
      decltype(std::declval<T>.end()) mEnd;
      
   public:
      template<typename T1>
      explicit AnyRangeImpl(T1&& t)
       : mContainerRef(std::forward<T1>(t)), mEnd(mContainerRef.end())
      {}
      
      bool hasNext() override final      
      {
         if (mItr)
            ++(*mItr);
         else
            mItr = mContainerRef.begin();
         
         return *mItr != mEnd;
      }
      
      ValueType next() override final      
      {
         assert(*mItr != mEnd);
         return *mItr;
      }
   };
   
   template<typename T>
   AnyRange<decltype(*std::declvalay_t<const T>().begin())> toAnyRange(T&& t)
   {
      return std::make_unique<AnyRangeImpl<const std::decay_t<T>>>(std::forward<T>(t));
   }
}

