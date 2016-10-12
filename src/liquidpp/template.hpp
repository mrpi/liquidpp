#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <memory>

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

    struct Comment;
    struct Conditional;
    struct UnevaluatedTag;
    
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
       Comment = 3
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
      case NodeType::Comment:
         return os << "Comment";
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
       BlockBody body;
    };
    
    struct Comment : public Block
    {
       void render(Context& context, std::string& res) const override final
       {
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
                   boost::get<Variable>(node).render(mutableScopedContext, res);
                   break;
                }
                case NodeType::Comment:
                {
                   auto& renderable = *boost::get<std::shared_ptr<const IRenderable>>(node);
                   renderable.render(mutableScopedContext, res);
                   break;
                }
                case NodeType::UnevaluatedTag:
                   break;
             }
          }
          
          return res;
       }
    };
    
    struct Conditional
    {
       std::string condition;
       BlockBody then;       
       
       auto tie() const
       {
          return std::tie(condition, then);
       }
       
       bool operator==(const Conditional& other) const
       {
          return tie() == other.tie();
       }
                     
       friend std::ostream& operator<<(std::ostream& os, const Conditional& con)
       {
          return os << "if (" << con.condition << ") {" << con.then << "}";
       }
    };

}
