#pragma once

#include <memory>
#include <type_traits>
#include <cassert>

#include <boost/optional.hpp>

namespace liquidpp
{
   template<typename ValueType>
   class AnyRange
   {
   private:
      class ImplBase
      {
      public:
         virtual ~ImplBase()
         {}
         
         virtual bool hasNext() = 0;
      
         virtual ValueType next() = 0;
      };
      std::shared_ptr<ImplBase> mImpl;
      
      template<typename T>
      class Impl : public ImplBase
      {
         private:
            T mContainerRef;
            boost::optional<decltype(std::declval<T>().begin())> mItr;
            decltype(std::declval<T>().end()) mEnd;
            
         public:
            template<typename T1>
            explicit Impl(T1&& t)
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
               return **mItr;
            }
        
      };
      
   public:
      template<typename T>
      static AnyRange<ValueType> from(T&& t)
      {
         AnyRange res;
         // no make_shared for private classes
         res.mImpl = std::shared_ptr<ImplBase>(new Impl<std::remove_reference_t<T>>(std::forward<T>(t)));
         return res;
      }
      
      template<typename T>
      static AnyRange<ValueType> reference(T& t)
      {
         AnyRange res;
         // no make_shared for private classes
         res.mImpl = std::shared_ptr<ImplBase>(new Impl<T&>(t));
         return res;
      }
      
      bool hasNext()
      {
         if (mImpl == nullptr)
            return false;
         return mImpl->hasNext();
      }
      
      ValueType next()
      {
         return mImpl->next();
      }
   };
}

