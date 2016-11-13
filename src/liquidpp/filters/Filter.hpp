#pragma once

#include "../Accessor.hpp"
#include "../Exception.hpp"

namespace liquidpp
{
class Context;

namespace filters
{

using FilterArgs = SmallVector<Value, 2>;

struct Filter 
{
   using Func0Arg = std::function<Value (Value&&)>;
   using Func1Arg = std::function<Value (Value&&, Value&&)>;
   using Func2Arg = std::function<Value (Value&&, Value&&, Value&&)>;
   
   using FuncContext0Arg = std::function<Value (liquidpp::Context&, Value&&)>;
   using FuncContext1Arg = std::function<Value (liquidpp::Context&, Value&&, Value&&)>;
   using FuncContext2Arg = std::function<Value (liquidpp::Context&, Value&&, Value&&, Value&&)>;
   
   using FilterFunction = boost::variant<Func0Arg, Func1Arg, Func2Arg, FuncContext0Arg, FuncContext1Arg, FuncContext2Arg>;
   FilterFunction mFunction;
   
   template<typename T>
   static constexpr bool is0ArgFunction = std::is_convertible<T, Func0Arg>::value;
   
   template<typename T>
   static constexpr bool is1ArgFunction = std::is_convertible<T, Func1Arg>::value;
   
   template<typename T>
   static constexpr bool is2ArgFunction = std::is_convertible<T, Func2Arg>::value;
   
   template<typename T>
   static constexpr bool isContext0ArgFunction = std::is_convertible<T, FuncContext0Arg>::value;
   
   template<typename T>
   static constexpr bool isContext1ArgFunction = std::is_convertible<T, FuncContext1Arg>::value;
   
   template<typename T>
   static constexpr bool isContext2ArgFunction = std::is_convertible<T, FuncContext2Arg>::value;
   
   Filter() = default;
   
   template<typename T>
   Filter(T func, std::enable_if_t<is0ArgFunction<T>, void**> = 0)
     : mFunction(Func0Arg{func})
   {}
   
   template<typename T>
   Filter(T func, std::enable_if_t<is1ArgFunction<T>, void**> = 0)
     : mFunction(Func1Arg{func})
   {}
   
   template<typename T>
   Filter(T func, std::enable_if_t<is2ArgFunction<T>, void**> = 0)
     : mFunction(Func2Arg{func})
   {}
   
   template<typename T>
   Filter(T func, std::enable_if_t<isContext0ArgFunction<T>, void**> = 0)
     : mFunction(FuncContext0Arg{func})
   {}
   
   template<typename T>
   Filter(T func, std::enable_if_t<isContext1ArgFunction<T>, void**> = 0)
     : mFunction(FuncContext1Arg{func})
   {}
   
   template<typename T>
   Filter(T func, std::enable_if_t<isContext2ArgFunction<T>, void**> = 0)
     : mFunction(FuncContext2Arg{func})
   {}
   
   static void assureSize(FilterArgs& args, size_t len)
   {
      enforce(args.size() <= len, "Too many arguments for this filter!");
      
      while(args.size() < len)
         args.emplace_back();
   }
   
   Value operator()(liquidpp::Context& c, Value&& val, FilterArgs&& args) const
   {
      switch(mFunction.which())
      {
      case 0:
      {
         assureSize(args, 0);
         auto&& f = boost::get<Func0Arg>(mFunction);
         return f(std::move(val));
      }
      case 1:
      {
         assureSize(args, 1);
         auto&& f = boost::get<Func1Arg>(mFunction);
         return f(std::move(val), std::move(args[0]));
      }
      case 2:
      {
         assureSize(args, 2);
         auto&& f = boost::get<Func2Arg>(mFunction);
         return f(std::move(val), std::move(args[0]), std::move(args[1]));
      }
      case 3:
      {
         assureSize(args, 0);
         auto&& f = boost::get<FuncContext0Arg>(mFunction);
         return f(c, std::move(val));
      }
      case 4:
      {
         assureSize(args, 1);
         auto&& f = boost::get<FuncContext1Arg>(mFunction);
         return f(c, std::move(val), std::move(args[0]));
      }
      case 5:
      {
         assureSize(args, 2);
         auto&& f = boost::get<FuncContext2Arg>(mFunction);
         return f(c, std::move(val), std::move(args[0]), std::move(args[1]));
      }
      default:
         assert(false);
         throw std::runtime_error("Unhandled function type in variant!");
      }
   }
         
   explicit operator bool() const
   {
      return mFunction.which() != 0 || boost::get<Func0Arg>(mFunction);
   }

};

}
}
