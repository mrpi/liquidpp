#include "Template.hpp"

#include "Context.hpp"

namespace liquidpp {

std::ostream &operator<<(std::ostream &os, NodeType t) {
  switch (t) {
  case NodeType::String:
    return os << "String";
  case NodeType::Variable:
    return os << "Variable";
  case NodeType::UnevaluatedTag:
    return os << "UnevaluatedTag";
  case NodeType::Tag:
    return os << "Tag";
  }

  assert(false);
  return os << "<unknown>";
}

std::string Template::operator()(const Context &context) const {
  try {
    std::string res;
    Context mutableScopedContext{&context};

    for (auto &&node : root.nodeList)
      renderNode(mutableScopedContext, node, res);

    return res;
  } catch (Exception &e) {
    e.position() = findPosition(e.errorPart());
    throw;
  }
}

Exception::Position Template::findPosition(string_view needle) const {
  Exception::Position res;
  auto &templ = root.templateRange;

  if (needle.data() < templ.data())
    return res;
  if (needle.data() >= templ.data() + templ.size())
    return res;

  res = {1, 1};
  size_t needleIdx = needle.data() - templ.data();
  for (size_t i = 0; i < needleIdx; i++) {
    if (templ[i] == '\n') {
      res.line++;
      res.column = 1;
    } else
      res.column++;
  }

  return res;
}
}
