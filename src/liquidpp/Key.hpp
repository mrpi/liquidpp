#pragma once

#include "config.h"

#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>

namespace liquidpp
{
   using OptIndex = boost::optional<size_t>;

   struct Key
   {
      string_view name;
      OptIndex idx;

      explicit operator bool() const
      {
         return !name.empty();
      }
   };

   using Path = std::vector<Key>;

   inline Key popKey(string_view& path)
   {
      Key res;

      auto pos = path.find('.');
      if (pos == std::string::npos)
      {
         res.name = path;
         path = string_view{};
      }
      else
      {
         res.name = path.substr(0, pos);
         path.remove_prefix(pos + 1);
      }

      auto idxStart = res.name.find('[');
      if (idxStart != std::string::npos && res.name.back() == ']')
      {
         auto size = res.name.size();
         res.idx = boost::lexical_cast<size_t>(res.name.substr(idxStart + 1, size - idxStart - 2));
         res.name.remove_suffix(size - idxStart);
      }

      return res;
   }

   inline string_view popLastKey(string_view& path)
   {
      auto pos = path.find_last_of(".");
      if (pos == std::string::npos)
      {
         auto res = path;
         path = string_view{};
         return res;
      }

      auto res = path.substr(pos+1);
      path.remove_suffix(res.size());
      return res;
   }
}
