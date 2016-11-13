#pragma once

#include <memory>

#include "filters/Filter.hpp"

namespace liquidpp
{

struct FilterFactory {
   filters::Filter operator()(string_view name) const;
};

}
