#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <regex>

#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/get.hpp>

#include "config.h"
#include "IRenderable.hpp"

namespace liquidpp
{
    struct Variable : public IRenderable
    {
       std::string leftPadding;
       std::string variableName;
       std::string filters;
       
       auto tie() const
       {
          return std::tie(leftPadding, variableName, filters);
       }
       
       bool operator==(const Variable& other) const
       {
          return tie() == other.tie();
       }
       
       friend std::ostream& operator<<(std::ostream& os, const Variable& obj)
       {
          return os << "{{" << obj.leftPadding << obj.variableName << obj.filters << "}}";
       }
       
       void render(Context& context, std::string& out) const override final
       {
          auto&& val = context.get(variableName);
          if (val)
             out += *val;
       }
    };
    
    struct Tag : public IRenderable
    {
       std::string leftPadding;
       std::string name;
       std::string value;
       
       auto tie() const
       {
          return std::tie(leftPadding, name, value);
       }
       
       bool operator==(const Tag& other) const
       {
          return tie() == other.tie();
       }
                     
       friend std::ostream& operator<<(std::ostream& os, const Tag& tag)
       {
          return os << "{%" << tag.leftPadding << tag.name << tag.value << "%}";
       }
    };
    
    struct UnevaluatedTag : public Tag
    {
       void render(Context& context, std::string& out) const override final
       {
          // TODO: assert? or throw?
       }
    };

    using Node = boost::variant<std::string, Variable, UnevaluatedTag, std::shared_ptr<const IRenderable>>;
    
    enum class NodeType
    {
       String = 0,
       Variable = 1,
       UnevaluatedTag = 2,
       Tag = 3
    };
        
   inline std::ostream& operator<<(std::ostream& os, NodeType t)
   {
      switch(t)
      {
      case NodeType::String:
         return os << "String";
      case NodeType::Variable:
         return os << "Variable";
      case NodeType::UnevaluatedTag:
         return os << "UnevaluatedTag";
      case NodeType::Tag:
         return os << "Tag";
      default:
         assert(false);
         return os << "<unknown>";
      }
   }

    inline NodeType type(const Node& n)
    {
       return static_cast<NodeType>(n.which());
    }
    
    struct BlockBody
    {
       std::vector<Node> nodeList;
       
       bool operator==(const BlockBody& other) const
       {
          return nodeList == other.nodeList;
       }
       
       friend std::ostream& operator<<(std::ostream& os, const BlockBody& obj)
       {
          bool first = true;

          os << '[';
          for(auto&& n : obj.nodeList)
          {
             if (first)
                first = false;
             else
                os << ", ";
             
             os << n;
          }
          os << ']';
          
          return os;
       }
    };
    
    struct Block : public Tag
    {
       Block(Tag&& tag)
        : Tag(std::move(tag))
       {}
       
       BlockBody body;
    };
    
    struct Comment : public Block
    {
       Comment(Tag&& tag)
        : Block(std::move(tag))
       {}
       
       void render(Context& /*context*/, std::string& /*res*/) const override final
       {
       }
    };

    inline void renderNode(Context& context, const Node& node, std::string& res)
    {
       // TODO: use static visitor
       switch(type(node))
       {
          case NodeType::String:
          {
             res += boost::get<std::string>(node);
             break;
          }
          case NodeType::Variable:
          {
             boost::get<Variable>(node).render(context, res);
             break;
          }
          case NodeType::Tag:
          {
             auto& renderable = *boost::get<std::shared_ptr<const IRenderable>>(node);
             renderable.render(context, res);
             break;
          }
          case NodeType::UnevaluatedTag:
             break;
       }
    }
    
    struct For : public Block
    {
       std::string loopVariable;
       std::string rangeVariable;
       
       For(Tag&& tag)
        : Block(std::move(tag))
       {
          std::regex rex{" *([^ ]+) +in +([^ ]+) *"};
          std::smatch match;
          if (!std::regex_match(value, match, rex))
             throw std::invalid_argument("Malformed 'for' tag ('" + value + "')!");

          loopVariable = match[1].str();
          rangeVariable = match[2].str();
       }
       
       void render(Context& context, std::string& res) const override final
       {
          auto val = context.get(rangeVariable);
          if (val)
             return;

          if (boost::get<ValueTag>(val) != ValueTag::Range)
             return;

          size_t idx = 0;
          Value currentVal;
          while (true) {
             std::string idxPath = rangeVariable + '[' + boost::lexical_cast<std::string>(idx++) + ']';
             currentVal = context.get(idxPath);
             Context loopVarContext{context};

             if (currentVal)
                loopVarContext.set(loopVariable, *currentVal);
             else if (boost::get<ValueTag>(currentVal) != ValueTag::OutOfRange)
                loopVarContext.setReference(loopVariable, idxPath);
             else
                break;

             for(auto&& node : body.nodeList)
                renderNode(loopVarContext, node, res);
          }
       }
    };

    struct Template
    {
       BlockBody root;
       
       std::string operator()(const Context& context) const
       {
          std::string res;
          Context mutableScopedContext{&context};
                    
          for(auto&& node : root.nodeList)
             renderNode(mutableScopedContext, node, res);

          return res;
       }
    };
    
    struct If : public Block
    {
       If(Tag&& tag)
        : Block(std::move(tag))
       {}

       friend std::ostream& operator<<(std::ostream& os, const If& con)
       {
          return os << "if (" << con.value << ") {" << /*TODO* ??? << */ "}";
       }
       
       void render(Context& context, std::string& res) const override final
       {

       }
    };

    struct TagFactory
    {
       std::shared_ptr<Tag> operator()(UnevaluatedTag&& tag) const
       {
          if (tag.name == "for")
             return std::make_shared<For>(std::move(tag));
          if (tag.name == "if")
             return std::make_shared<If>(std::move(tag));
          if (tag.name == "comment")
             return std::make_shared<Comment>(std::move(tag));
          
          return nullptr;
       }
    };
}
