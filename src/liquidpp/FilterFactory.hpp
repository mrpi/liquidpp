#pragma once

#include <memory>

#include "filters/Filter.hpp"

namespace liquidpp
{

struct FilterFactory {
   std::shared_ptr<filters::Filter> operator()(string_view name) const;
};

}
