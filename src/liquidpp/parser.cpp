#include "parser.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>

#include <boost/algorithm/string/trim.hpp>

#include <iostream>
#include <fstream>
#include <type_traits>

// We need to tell fusion about our Template struct
// to make it a first-class fusion citizen
BOOST_FUSION_ADAPT_STRUCT(
   liquidpp::Variable,
   (std::string, leftPadding)
      (std::string, variableName)
      (std::string, filters)
)

BOOST_FUSION_ADAPT_STRUCT(
   liquidpp::BlockBody,
   (std::vector<liquidpp::Node>, nodeList)
)

BOOST_FUSION_ADAPT_STRUCT(
   liquidpp::UnevaluatedTag,
   (std::string, leftPadding)
      (std::string, name)
      (std::string, value)
)

namespace liquidpp {
namespace impl {
namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;
namespace lex = boost::spirit::lex;
namespace ascii = boost::spirit::ascii;

template<typename Iterator>
struct LiquidGrammer
   : qi::grammar<Iterator, BlockBody()> {
   LiquidGrammer(std::ostream& errorOut)
      : LiquidGrammer() {
      using qi::on_error;
      using qi::fail;
      using namespace qi::labels;

      using phoenix::construct;
      using phoenix::val;

      on_error<fail>
         (
            blockBody, errorOut
               << val("Error! Expecting ")
               << _4                               // what failed?
               << val(" here: \"")
               << construct<std::string>(_3, _2)   // iterators to error-pos, end
               << val("\"")
         );

   }

   LiquidGrammer()
      : LiquidGrammer::base_type(blockBody, "blockBody") {
      using qi::lit;
      using qi::lexeme;
      using qi::raw;
      using qi::eps;
      using ascii::char_;

      using phoenix::push_back;

      text %= raw[+(char_ - "{{" - "{%")];
      variable %=
         "{{"
         > raw[*lit(' ')]             // leftPadding
         > raw[+(char_ - "}}" - ' ' - '|')] // variableName
         > raw[*(char_ - "}}")]       // filters
         > "}}";
      tag %=
         "{%"
         > raw[*lit(' ')]             // leftPadding
         > raw[+(char_ - "%}" - ' ')] // name
         > raw[*(char_ - "%}")]       // value
         > "%}";

      node %= text | variable | tag;
      blockBody %= *node > eps;

      variable.name("variable");
      text.name("text");
      tag.name("tag");
      node.name("node");
      blockBody.name("blockBody");
   }

   qi::rule<Iterator, BlockBody()> blockBody;
   qi::rule<Iterator, Node()> node;
   qi::rule<Iterator, UnevaluatedTag(), qi::locals<std::string>> tag;
   qi::rule<Iterator, Variable()> variable;
   qi::rule<Iterator, std::string()> text;
};

BlockBody parseFlat(string_view content) {
   BlockBody flatNodes;

   using Grammer = const LiquidGrammer<string_view::const_iterator>;
   static Grammer liquidGrammer;

   auto iter = content.begin();
   auto end = content.end();

   bool r = parse(iter, end, liquidGrammer, flatNodes);
   if (!r) {
      // We parse again with error message output
      std::ostringstream errorOut;
      Grammer liquidGrammer{errorOut};
      auto iter = content.begin();
      auto end = content.end();

      BlockBody flatNodes;
      parse(iter, end, liquidGrammer, flatNodes);
      throw std::runtime_error("Parsing failed! " + errorOut.str());
   }

   return flatNodes;
}

}
}

