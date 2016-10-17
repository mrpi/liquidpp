#include "If.hpp"

#include <regex>

namespace liquidpp {

If::If(Tag&& tag)
   : Block(std::move(tag)) {
   std::regex rex1(" *([^ !<>=\"']+) *(([=<>!]+) *'([^']*)'){0,1} *");
   std::smatch match1;
   if (std::regex_match(value, match1, rex1)) {
      variable = match1[1].str();
      std::string comWith = match1[2].str();
      if (!comWith.empty()) {
         std::regex rex2("([=<>!]+) *'([^']*)'");
         std::smatch match2;
         std::regex_match(comWith, match2, rex2);
         compareWith.operator_ = match2[1].str();
         compareWith.value = match2[2].str();
         if (!operatorIsValid(compareWith.operator_))
            throw std::runtime_error("Invalid operator '" + compareWith.operator_ + "' in 'if' tag ('" + value + "')!");
      }
   } else
      throw std::invalid_argument("Malformed 'if' tag ('" + value + "')!");
}

void If::render(Context& context, std::string& res) const {

}

bool If::operatorIsValid(const std::string& operator_) {
   for (auto& opr : {"==", "!=", "<", "<=", ">", ">="}) {
      if (operator_ == opr)
         return true;
   }
   return false;
}

std::ostream& operator<<(std::ostream& os, const If& con) {
   return os << "if (" << con.value << ") {" << /*TODO* ??? << */ "}";
}

}
