#include "Key.hpp"

#include "Expression.hpp"
#include "Misc.hpp"

namespace liquidpp
{

template<char OpeningBracket, char ClosingBracket>
inline size_t matchingBracket(const string_view path)
{
   size_t openCnt = 1;
   assert(path.empty() || path[0] == OpeningBracket);   
   
   for (size_t i=1; i < path.size(); i++)
   {
      switch(path[i])
      {
         case OpeningBracket:
            openCnt++;
            break;
         case ClosingBracket:
            openCnt--;
            if (openCnt == 0)
               return i;
            break;            
      }
   }
   
   return std::string::npos;
}
   
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
       idxEnd = matchingBracket<'[', ']'>(path);
       if (idxEnd == std::string::npos)
          throw Exception("Unterminated array index!", path.substr(0, 1));
       if (idxEnd < 1)
          throw Exception("Empty array index!", path.substr(0, 1));
       auto idxPart = string_view{path.data() + 1, idxEnd - 1};
       
       if (idxPart[0] >= '0' && idxPart[0] <= '9')
          res = Key{lex_cast<size_t>(idxPart, "Array index is not a valid integral value!")};
       else
          res = Key{toPath(idxPart)};
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
