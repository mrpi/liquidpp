// runner file contents
#define NONIUS_RUNNER
#include <nonius/nonius_single.h++>

#include <thread>
#include <iostream>

#include <liquidpp.hpp>

auto renderNoCaching = [](){
    liquidpp::Context c;
    c.set("name", "Donald Drumpf");
    return liquidpp::parse("Hello {{name}}!")(c);
};

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

NONIUS_BENCHMARK("Hello {{name}}! (no caching)", [](nonius::chronometer meter){
   renderNoCaching();
   renderNoCaching();
   
   meter.measure(renderNoCaching);
});

NONIUS_BENCHMARK("Hello {{name}}! (cached context)", [](nonius::chronometer meter) {
    renderNoCaching();
    renderNoCaching();
   
    liquidpp::Context c;
    c.set("name", "Donald Drumpf");
    meter.measure([&](){ return liquidpp::parse("Hello {{name}}!")(c);});
})

NONIUS_BENCHMARK("Hello {{name}}! (cached template)", [](nonius::chronometer meter) {
    renderNoCaching();
    renderNoCaching();
   
    auto template_ = liquidpp::parse("Hello {{name}}!");
    meter.measure([&](){ 
       liquidpp::Context c;
       c.set("name", "Donald Drumpf");
       return template_(c);       
   });
})

NONIUS_BENCHMARK("Hello {{name}}! (cached context and template)", [](nonius::chronometer meter) {
   renderNoCaching();
   renderNoCaching();
   
    liquidpp::Context c;
    c.set("name", "Donald Drumpf");
    auto template_ = liquidpp::parse("Hello {{name}}!");
    meter.measure([&](){ return template_(c); });
})

NONIUS_BENCHMARK("Render date now", []() {
   liquidpp::Context c;
   auto template_ = liquidpp::parse("{{ 'now' | date: '%Y-%m-%d %H:%M:%s' }}!");
   return template_(c);
})

#ifdef LIQUIDPP_OLD_DATE_IMPL
NONIUS_BENCHMARK("Render date_old_impl now", []() {
   liquidpp::Context c;
   auto template_ = liquidpp::parse("{{ 'now' | date_old_impl: '%Y-%m-%d %H:%M:%s' }}!");
   return template_(c);
})
#endif

#else

int main()
{
   std::cerr << "Size of liquidpp::filters::Filter:         " << sizeof(liquidpp::filters::Filter) << '\n';
   std::cerr << "Size of liquidpp::RangeDefinition:         " << sizeof(liquidpp::RangeDefinition) << '\n';
   std::cerr << "Size of liquidpp::Value:                   " << sizeof(liquidpp::Value) << '\n';
   std::cerr << "Size of liquidpp::Path:                    " << sizeof(liquidpp::Path) << '\n';
   std::cerr << "Size of liquidpp::PathRef:                 " << sizeof(liquidpp::PathRef) << '\n';
   std::cerr << "Size of liquidpp::Expression::Token:       " << sizeof(liquidpp::Expression::Token) << '\n';
   std::cerr << "Size of liquidpp::Expression::FilterData:  " << sizeof(liquidpp::Expression::FilterData) << '\n';
   std::cerr << "Size of liquidpp::Expression::FilterChain: " << sizeof(liquidpp::Expression::FilterChain) << '\n';
   std::cerr << "Size of liquidpp::UnevaluatedTag:          " << sizeof(liquidpp::UnevaluatedTag) << '\n';
   std::cerr << "Size of liquidpp::Variable:                " << sizeof(liquidpp::Variable) << '\n';
   std::cerr << "Size of liquidpp::Node:                    " << sizeof(liquidpp::Node) << '\n';
   std::cerr << "Size of liquidpp::Template:                " << sizeof(liquidpp::Template) << '\n';
   std::cerr << "Size of liquidpp::Context:                 " << sizeof(liquidpp::Context) << '\n';
   
   auto func = 
      []{
         for (int i=0; i < 50000; i++)
         {
            auto val = renderNoCaching();
            if (val.empty())
               throw std::runtime_error("invalid state!");
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
