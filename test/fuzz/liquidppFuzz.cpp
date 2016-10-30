
#include <liquidpp.hpp>

#include "../../benchmark/ExampleData.hpp"

extern "C"
int LLVMFuzzerTestOneInput(const char* Data, size_t Size)
{
   liquidpp::Template templ;
   try {
      templ = liquidpp::parse({Data, Size});
   } catch(liquidpp::Exception& e) {
      auto endOfInput = Data + Size;
      if (e.errorPart().data() < Data || e.errorPart().data() >= endOfInput)
      {
         std::cerr << "Error part starts out of range (pointer: "
                   << static_cast<const void*>(e.errorPart().data()) << "-" << static_cast<const void*>(e.errorPart().data()+e.errorPart().size())
            << ", template range: " << static_cast<const void*>(Data) << "-" << static_cast<const void*>(endOfInput) << ")" << std::endl;
         __builtin_trap();
      }

      auto endOfErrorPart = e.errorPart().data() + e.errorPart().size();
      if (endOfErrorPart < Data || endOfErrorPart > endOfInput)
      {
         std::cerr << "Error part ends out of range (error range: " << static_cast<const void*>(e.errorPart().data())
                   << ", template range: " << static_cast<const void*>(Data) << "-" << static_cast<const void*>(endOfInput) << ")" << std::endl;
         __builtin_trap();
      }
   } catch(std::exception& e) {
      std::cerr << "std::exception on template parsing (" << e.what() << ")" << std::endl;
      __builtin_trap();
   }

   std::string res;
   try {
      res = templ(liquidpp::example_data::liquidContext());
   } catch(std::exception& e) {
   }

   return 0;  // Non-zero return values are reserved for future use.
}
