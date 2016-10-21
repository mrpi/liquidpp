// runner file contents
#define NONIUS_RUNNER
//#include <nonius/nonius_single.h++>

#include <thread>

#include <liquidpp.hpp>

#ifdef NONIUS_HPP

// We use std::ostringstream as our baseline.
// Not the toughest competitor out there but wideley used.
NONIUS_BENCHMARK("Hello {{name}}! (std::ostringstream)", [](nonius::chronometer meter) {
    //std::unordered_map<std::string, std::string> map{{"name", "Donald Drumpf"}};
    std::string name = "Donald Drumpf";
    meter.measure([&](){
       std::ostringstream oss;
       oss << "Hello " << name << "!";
       return oss.str();
    });
})

NONIUS_BENCHMARK("Hello {{name}}! (no caching)", []{
    liquidpp::Context c;
    c.set("name", "Donald Drumpf");
    return liquidpp::parse("Hello {{name}}!")(c);
});

NONIUS_BENCHMARK("Hello {{name}}! (cached context)", [](nonius::chronometer meter) {
    liquidpp::Context c;
    c.set("name", "Donald Drumpf");
    meter.measure([&](){ return liquidpp::parse("Hello {{name}}!")(c);});
})

NONIUS_BENCHMARK("Hello {{name}}! (cached template)", [](nonius::chronometer meter) {
    auto template_ = liquidpp::parse("Hello {{name}}!");
    meter.measure([&](){ 
       liquidpp::Context c;
       c.set("name", "Donald Drumpf");
       return template_(c);       
   });
})

NONIUS_BENCHMARK("Hello {{name}}! (cached context and template)", [](nonius::chronometer meter) {
    liquidpp::Context c;
    c.set("name", "Donald Drumpf");
    auto template_ = liquidpp::parse("Hello {{name}}!");
    meter.measure([&](){ return template_(c); });
})

#else

int main()
{
   auto func = 
      []{
         for (int i=0; i < 50000; i++)
         {
         liquidpp::Context c;
         c.set("name", "Donald Drumpf");
         volatile auto val = liquidpp::parse("Hello {{name}}!")(c);
         }
      };
   
#if 1
   func();
#else
   std::vector<std::thread> threads(1);
   
   for(auto&& t : threads)
      t = std::thread(func);
   
   for(auto&& t : threads)
      t.join();
#endif
}
#endif
