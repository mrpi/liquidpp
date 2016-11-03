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
   
   
}
