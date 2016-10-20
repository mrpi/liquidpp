#pragma once

namespace liquidpp
{

class Exception : public std::runtime_error
{
private:
   string_view mErrorPart;

public:
   Exception(const std::string& msg, string_view errorPart)
      : std::runtime_error(msg), mErrorPart(errorPart)
   {}

   string_view errorPart() const
   {
      return mErrorPart;
   }
};

};