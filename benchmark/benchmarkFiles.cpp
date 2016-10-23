#include <liquidpp.hpp>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

#include <iostream>
#include <fstream>
#include <iterator>

void loadDatabase(rapidjson::Document& doc)
{
   std::ifstream is("vision.database.json");
   if (is)
   {
      std::string jsonStr(std::istreambuf_iterator<char>(is), {});


      doc.Parse(jsonStr.data());
      return;
   }

   throw std::runtime_error("Could not load database file 'vision.database.json'!");
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

int main(int argc, char* args[])
{
   if (argc < 2)
   {
      std::cerr << "Usage: " << args[0] << " " << std::endl;
      return 1;
   }

   std::ifstream is(args[1]);
   if (!is)
   {
      std::cerr << "Could not open file '" << args[1] << "''" << std::endl;
      return 1;
   }
   std::string templateContent(std::istreambuf_iterator<char>(is), {});;

   rapidjson::Document doc;
   loadDatabase(doc);

   liquidpp::Context c;
   c.setAnonymous(doc);
   setupDefaults(c);

   try {
      //for (int i=0; i < 10000; i++)
      {
         auto rendered = liquidpp::render(templateContent, c);
         if (rendered.empty())
            std::cerr << "Rendering returned empty template!" << std::endl;
         std::cout << rendered << std::endl;
      }
   }
   catch(liquidpp::Exception& e)
   {
      std::cerr << "liquidpp error: " << e.what() << std::endl;
      std::cerr << "Error at: " << e.errorPart() << std::endl;
   }
}
