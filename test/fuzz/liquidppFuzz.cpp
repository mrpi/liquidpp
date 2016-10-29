
#include <liquidpp.hpp>

#include <iostream>
#include <fstream>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

auto& rapidJsonExample()
{
   static rapidjson::Document doc;

   std::ifstream is("../benchmark/vision.database.json");
   if (!is)
      throw std::runtime_error("Failed to open vision database!");

   std::string templateContent(std::istreambuf_iterator<char>(is), {});
   doc.Parse(templateContent.c_str());

   return doc;
}

// Some standard direct accessors so that the specialized templates
// render correctly
void setupDefaults(liquidpp::Context& c)
{
   c.setLink("collection", "collections[0]");
   c.setLink("product", "products[0]");
   c.setLink("blog", "blogs[0]");
   c.setLink("article", "blog.articles[0]");
}

auto initLiquidContext()
{
   liquidpp::Context c;
   c.setAnonymous(rapidJsonExample());
   setupDefaults(c);
   return c;
}

auto& liquidContext()
{
   static liquidpp::Context c = initLiquidContext();
   return c;
}

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
      res = templ(liquidContext());
   } catch(std::exception& e) {
   }

   return 0;  // Non-zero return values are reserved for future use.
}
