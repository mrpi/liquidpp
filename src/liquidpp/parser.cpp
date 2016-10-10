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

namespace liquidpp
{
   namespace fusion = boost::fusion;
   namespace phoenix = boost::phoenix;
   namespace qi = boost::spirit::qi;
   namespace lex = boost::spirit::lex;
   namespace ascii = boost::spirit::ascii;
   
    template <typename Iterator>
    struct LiquidGrammer
      : qi::grammar<Iterator, BlockBody()>
    {
        LiquidGrammer(std::ostream& errorOut)
          : LiquidGrammer::base_type(blockBody, "blockBody")
        {
            using qi::lit;
            using qi::lexeme;
            using qi::raw;
            using qi::eps;
            using qi::on_error;
            using qi::fail;
            using ascii::char_;
            using namespace qi::labels;

            using phoenix::construct;
            using phoenix::val;
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

            on_error<fail>
            (
                blockBody
              , errorOut
                    << val("Error! Expecting ")
                    << _4                               // what failed?
                    << val(" here: \"")
                    << construct<std::string>(_3, _2)   // iterators to error-pos, end
                    << val("\"")
            );
        }

        qi::rule<Iterator, BlockBody()> blockBody;
        qi::rule<Iterator, Node()> node;
        qi::rule<Iterator, UnevaluatedTag(), qi::locals<std::string>> tag;
        qi::rule<Iterator, Variable()> variable;
        qi::rule<Iterator, std::string()> text;
    };
    
    bool isBlockNode(const std::string& name)
    {
       for(auto blockName : {"comment", "if"})
       {
          if (name == blockName)
             return true;
       }
       
       return false;
    }
    
    template<typename Iterator>
    BlockBody buildBlocks(Iterator& itr, const Iterator& end, const std::string& endTagName = "")
    {
       BlockBody res;
       
       for(; itr != end; ++itr)
       {
          auto&& node = *itr;
          
          if (type(node) == NodeType::UnevaluatedTag)
          {
             auto&& tag = boost::get<UnevaluatedTag>(node);
             auto&& name = tag.name;
             if (name == endTagName)
                break;
             if (isBlockNode(name))
             {
                auto comment = std::make_shared<Comment>();
                comment->name = tag.name;
                comment->value = tag.value;
                comment->body = buildBlocks(++itr, end, "end"+name);
                res.nodeList.emplace_back(std::shared_ptr<IRenderable>(std::move(comment)));
                continue;
             }
          }

          res.nodeList.push_back(std::move(node));
       }
       
       if (itr == end && !endTagName.empty())
          throw std::runtime_error("Missing closing tag '{%" + endTagName + "%}'");
          
       return res;
    }

   Template parse(const std::string& content)
   {
      liquidpp::Template ast;
      
      std::ostringstream errorOut;
      typedef liquidpp::LiquidGrammer<std::string::const_iterator> LiquidGrammer;
      LiquidGrammer liquidGrammer{errorOut};

      auto iter = content.begin();
      auto end = content.end();
      
      BlockBody flatNodes;
      bool r = parse(iter, end, liquidGrammer, flatNodes);
      if (!r)
         throw std::runtime_error("Parsing failed! " + errorOut.str());
      
      auto itr = flatNodes.nodeList.begin();
      ast.root = buildBlocks(itr, flatNodes.nodeList.end());

      return ast;
   }   
}

