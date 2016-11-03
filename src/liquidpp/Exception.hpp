#pragma once

#include "config.h"

#include <sstream>

namespace liquidpp {

class Exception : public std::runtime_error {
public:
  struct Position {
    size_t line{0};
    size_t column{0};

    Position() = default;

    Position(size_t line, size_t column) : line(line), column(column) {}

    std::string toString() const {
      if (line == 0)
        return "<Position in template is not available>";

      std::ostringstream oss;
      oss << "Line: " << line << ", Column: " << column;
      return oss.str();
    }
  };

private:
  string_view mErrorPart;
  Position mPosition;

public:
  Exception(const std::string &msg, string_view errorPart)
      : std::runtime_error(msg), mErrorPart(errorPart) {}

  string_view errorPart() const { return mErrorPart; }

  const Position &position() const { return mPosition; }

  Position &position() { return mPosition; }
};
};
