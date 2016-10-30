#include <liquidpp.hpp>

#include "ExampleData.hpp"

#include <iterator>

int main(int argc, char* args[])
{
   if (argc < 2)
   {
      std::cerr << "Usage: " << args[0] << " <liquid template file>" << std::endl;
      return 1;
   }

   std::ifstream is(args[1]);
   if (!is)
   {
      std::cerr << "Could not open file '" << args[1] << "''" << std::endl;
      return 1;
   }
   std::string templateContent(std::istreambuf_iterator<char>(is), {});;

   liquidpp::Context& c = liquidpp::example_data::liquidContext();

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
      std::cerr << e.position().toString() << std::endl;
   }
}
