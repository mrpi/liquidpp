#include "FilterFactory.hpp"

#include "filters/Filter.hpp"

#include "filters/Capitalize.hpp"
#include "filters/Escape.hpp"
#include "filters/Date.hpp"
#include "filters/Downcase.hpp"
#include "filters/Upcase.hpp"
#include "filters/Default.hpp"
#include "filters/Lstrip.hpp"
#include "filters/NewlineToBr.hpp"
#include "filters/Remove.hpp"
#include "filters/RemoveFirst.hpp"
#include "filters/Replace.hpp"
#include "filters/ReplaceFirst.hpp"
#include "filters/Round.hpp"
#include "filters/Rstrip.hpp"
#include "filters/Slice.hpp"
#include "filters/Strip.hpp"
#include "filters/StripNewlines.hpp"
#include "filters/Truncate.hpp"
#include "filters/TruncateWords.hpp"
#include "filters/StripHtml.hpp"
#include "filters/UrlEncode.hpp"
#include "filters/Split.hpp"
#include "filters/Join.hpp"
#include "filters/Sort.hpp"
#include "filters/Reverse.hpp"
#include "filters/Uniq.hpp"
#include "filters/Map.hpp"
#include "filters/NumberFilter.hpp"
#include "filters/FloatFilter.hpp"

namespace liquidpp
{

filters::Filter FilterFactory::operator()(string_view name) const
{
   using namespace filters;

   if (name == "abs")
      return makeNumberFilter( [](auto d){ return std::abs(d); } );
   if (name == "append")
      return [](Value&& val, Value&& toAppend) -> Value { return val.toString() + toAppend.toString(); };
   if (name == "capitalize")
      return Capitalize{};
   if (name == "ceil")
      return makeFloatFilter( [](double d){ return std::ceil(d); } );
   if (name == "date")
      return Date{};
   if (name == "default")
      return Default{};
   if (name == "divided_by")
      return makeNumberFilter1Arg( [](auto d, auto arg){ return d / arg; } );
   if (name == "downcase")
      return Downcase{};
   if (name == "escape")
      return Escape{};
   if (name == "escape_once")
      return EscapeOnce{};
   if (name == "floor")
      return makeFloatFilter( [](double d){ return std::floor(d); } );
   if (name == "join")
      return Join{};
   if (name == "lstrip")
      return Lstrip{};
   if (name == "map")
      return Map{};
   if (name == "minus")
      return makeNumberFilter1Arg( [](auto d, auto arg){ return d - arg; } );
   if (name == "modulo")
      return makeNumberFilter1Arg( [](auto d, auto arg){ return fmod(d, arg); } );
   if (name == "newline_to_br")
      return NewlineToBr{};
   if (name == "plus")
      return makeNumberFilter1Arg( [](auto d, auto arg){ return d + arg; } );
   if (name == "prepend")
      return [](Value&& val, Value&& prefix) -> Value { return prefix.toString() + val.toString(); };
   if (name == "remove")
      return Remove{};
   if (name == "remove_first")
      return RemoveFirst{};
   if (name == "replace")
      return Replace{};
   if (name == "replace_first")
      return ReplaceFirst{};
   if (name == "reverse")
      return Reverse{};
   if (name == "round")
      return Round{};
   if (name == "rstrip")
      return Rstrip{};
   if (name == "size")
      return [](Value&& val) -> Value {
         return val.size();
      };
   if (name == "slice")
      return Slice{};
   if (name == "sort")
      return Sort{};
   if (name == "split")
      return Split{};
   if (name == "strip")
      return Strip{};
   if (name == "strip_html")
      return StripHtml{};
   if (name == "strip_newlines")
      return StripNewlines{};
   if (name == "times")
      return makeNumberFilter1Arg( [](auto d, auto arg){ return d * arg; } );
   if (name == "truncate")
      return Truncate{};
   if (name == "truncatewords")
      return TruncateWords{};
   if (name == "uniq")
      return Uniq{};
   if (name == "upcase")
      return Upcase{};
   if (name == "url_encode")
      return UrlEncode{};

   return filters::Filter{};
}

}
