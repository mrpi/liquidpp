#pragma once

#include <liquidpp.hpp>

#include <iostream>
#include <fstream>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

namespace liquidpp
{
   
   namespace example_data
   {
      
      auto& rapidJsonExample()
      {
         static rapidjson::Document doc;

         const std::string jsonPath = "../benchmark/vision.database.json";
         std::ifstream is;
         is.open(jsonPath);
         if (!is)
         {
            is.open("../" + jsonPath);
            if (!is)
            {
               std::cerr << "Failed to open vision database file ('" << jsonPath << "')!" << std::endl;
               throw std::runtime_error("Failed to open vision database file ('" + jsonPath + "')!");
            }
         }

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

      bool initLiquidContext(liquidpp::Context& c)
      {
         c.setAnonymous(std::ref(rapidJsonExample()));
         setupDefaults(c);
         return true;
      }

      auto& liquidContext()
      {
         static liquidpp::Context c;
         static bool initialized = false;
         if (!initialized)
         {
            initLiquidContext(c);
            initialized = true;
         }
         return c;
      }

   }
   
}
