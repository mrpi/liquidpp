#include "Key.hpp"

#include "Expression.hpp"
#include "Misc.hpp"

namespace liquidpp
{

Key popKey(string_view &path) {
  if (path.empty())
    return Key{};
  
  bool isInternalKey = path[0] == '\0';

  if (path[0] == '[') {
    Key res;
    size_t idxEnd = std::string::npos;
     
    if (path[1] == '"' || path[1] == '\'')
    {
       auto pos = path.find(path[1], 2);
       if (path[pos+1] != ']')
          throw Exception("Expected closing bracket after end of string leteral!", path.substr(pos));
       idxEnd = pos+1;
       res = Key{path.substr(2, pos-2)};
    }
    else
    {
       idxEnd = path.find(']');
       if (idxEnd == std::string::npos)
          throw Exception("Unterminated array index!", path.substr(0, 1));
       auto numPart = string_view{path.data() + 1, idxEnd - 1};
       res = Key{lex_cast<size_t>(numPart, "Array index is not an integral value!")};
    }
     
    if (path.size() <= idxEnd+1)
       path = string_view{};
    else if (path[idxEnd + 1] == '[')
       path.remove_prefix(idxEnd + 1);
    else if (path[idxEnd + 1] == '.')
       path.remove_prefix(idxEnd + 2);
    else
       throw Exception("Expected '.' after array index!", path.substr(idxEnd+1, 1));
      
    return res;
  }

  for (auto &c : path) {
    if (c == '.' || c == '[') {
      auto keyStr = path.substr(0, &c - path.data());
      if (c == '.')
        path.remove_prefix(&c - path.data() + 1);
      else
        path.remove_prefix(&c - path.data());
      return Key{keyStr};
    }
    
    if (Expression::isAsciiAlpha(c) || Expression::isDigit(c) || c == '_' || c == '-')
       continue;
    
    if (isInternalKey)
       continue;
    
    throw Exception("Invalid character for variable name!", {&c, 1});
  }

  auto keyStr = path;
  path = string_view{};
  return Key{keyStr};
}
   
}
