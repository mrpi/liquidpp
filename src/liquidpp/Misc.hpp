#pragma once

#include "config.h"

#include <boost/lexical_cast.hpp>

namespace liquidpp
{
   
class Exception;

template<typename T>
auto lex_cast(string_view in, const char* msg = nullptr)
{
   try {
      return boost::lexical_cast<T>(in.data(), in.size());
   } catch(boost::bad_lexical_cast&) {
      throw Exception(msg ? msg : "Invalid value (failed to convert to expected type)!", in);
   }
}

// (see: https://en.wikipedia.org/wiki/UTF-8#Codepage_layout)
namespace utf8 {
   inline bool isSingleByteChar( unsigned char b ) {
      // Plain ASCII chars
      return b <= 0x7F;
   }
   
   inline bool isFollowByteInMultiByteChar(unsigned char b) {
      return b >= 0x80 && b <= 0xBF;
   }
   
   inline bool isFirstInTwoByteChar( unsigned char b ) {
      return b >= 0xC2 && b <= 0xDF;
   }
   
   inline bool isFirstInThreeByteChar( unsigned char b ) {
      return b >= 0xE0 && b <= 0xEF;
   }
   
   inline bool isFirstInFourByteChar( unsigned char b ) {
      return b >= 0xF0 && b <= 0xF4;
   }
   
   inline bool isInvalidByte( unsigned char b ) {
      return b == 0xC0 || b == 0xC1 || b >= 0xF5;
   }
   
   inline bool isValid( char const* str, size_t len ) {
      int outstandingBytesOfCurrentChar = 0;
      
      for( std::size_t i = 0; i < len; ++ i ) {
            unsigned char b = static_cast<unsigned char>( str[i] );
            
            switch( outstandingBytesOfCurrentChar )
            {
               case 0:
                  if( isSingleByteChar( b ) )
                        outstandingBytesOfCurrentChar = 0;
                  else if( isFirstInTwoByteChar( b ) )
                        outstandingBytesOfCurrentChar = 1;
                  else if( isFirstInThreeByteChar( b ) )
                        outstandingBytesOfCurrentChar = 2;
                  else if( isFirstInFourByteChar( b ) )
                        outstandingBytesOfCurrentChar = 3;
                  else
                        return false;
                  
                  break;
                  
               case 1:
               case 2:
               case 3:
                  if( !isFollowByteInMultiByteChar( b ) )
                        return false;
                  
                  outstandingBytesOfCurrentChar--;
                  break;
                  
               default:
                  // outstandingBytesOfCurrentChar is negative: got follow byte when start byte was expected
                  return false;
            }
                           
            // explicit negative check (should be fully redundant here)
            assert( isInvalidByte( b ) == false );
      }
      
      return outstandingBytesOfCurrentChar == 0;
   }
   
   inline bool isValid(string_view str) {
      return isValid(str.data(), str.size());
   }
   
   inline string_view popU8Char(string_view& sv)
   {
      if (sv.empty())
         return string_view{};
               
      auto c = sv[0];
      size_t len = 0;
      
      if (isSingleByteChar(c))
         len = 1;
      else if (isFirstInTwoByteChar(c))
         len = 2;
      else if (isFirstInThreeByteChar(c))
         len = 3;
      else if (isFirstInFourByteChar(c))
         len = 4;
      
      auto res = sv.substr(0, len);
      if (res.empty() || !isValid(res))
      {
         // If we get here, the input is obviously not utf8
         // we expect an single byte encoding
         len = 1;
         res = sv.substr(0, len);
      }
         
      sv.remove_prefix(len);
      return res;
   }
   
   inline boost::optional<char32_t> popU32Char(string_view& sv)
   {
      auto u8Char = popU8Char(sv);
       
      switch(u8Char.size())
      {
      case 0:
         return boost::none;
      case 1:
         return static_cast<char32_t>(u8Char[0]);
      case 2:
         return ((u8Char[0] << 6) & 0x7ff) + (u8Char[1] & 0x3f);
      case 3:
         return ((u8Char[0] << 12) & 0xffff)
              + ((u8Char[1] << 6) & 0xfff) 
              +  (u8Char[2] & 0x3f);
      case 4:
         return ((u8Char[0] << 18) & 0x1fffff)
              + ((u8Char[1] << 12) & 0x3ffff)
              + ((u8Char[2] << 6) & 0xfff)
              +  (u8Char[3] & 0x3f);
      }
      
      throw Exception("Utf8 parsing error!", sv);
   }
   
   template <typename Utf8Container>
   Utf8Container& append(Utf8Container& res, char32_t cp)
   {
      if (cp < 0x80)
         res += static_cast<uint8_t>(cp);
      else if (cp < 0x800) {
         res += static_cast<uint8_t>((cp >> 6) | 0xc0);
         res += static_cast<uint8_t>((cp & 0x3f) | 0x80);
      }
      else if (cp < 0x10000) {
         res += static_cast<uint8_t>((cp >> 12) | 0xe0);
         res += static_cast<uint8_t>(((cp >> 6) & 0x3f) | 0x80);
         res += static_cast<uint8_t>((cp & 0x3f) | 0x80);
      }
      else {
         res += static_cast<uint8_t>((cp >> 18) | 0xf0);
         res += static_cast<uint8_t>(((cp >> 12) & 0x3f) | 0x80);
         res += static_cast<uint8_t>(((cp >> 6) & 0x3f) | 0x80);
         res += static_cast<uint8_t>((cp & 0x3f) | 0x80);
      }
      
      return res;
}
   
   inline string_view substr(string_view sv, size_t startIdx, size_t len)
   {
      size_t pos = 0;
      
      while(pos < startIdx)
      {
         pos++;
         if (popU8Char(sv).empty())
            throw std::out_of_range("Out of range on utf8::substr!");
      }
      
      auto start = sv.data();
      for(size_t i=0; i < len; i++)
      {
         if (popU8Char(sv).empty())
            break;
      }
      
      return {start, static_cast<size_t>(sv.data() - start)};
   }
   
      
   inline size_t characterCount(string_view sv)
   {
      size_t len = 0;
      
      while(!popU8Char(sv).empty())
         len++;

      return len;
   }
}   
   
}
