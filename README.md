# liquidpp
C++ version of the [Liquid Markup Language](https://shopify.github.io/liquid/). It is build on "Modern C++" techniques (C++14).

Usage
-----

```C++
liquidpp::Context c;
c.set("name", "Donald Drumpf");
auto rendered = liquidpp::render("Hello {{name}}!", c);
REQUIRE("Hello Donald Drumpf!" == rendered);
```

Features
-----
* Extendable with your own value types
* Extendable with your own reflection/container types
  (support for std::vector, std::map, std::tuple, boost::variant, boost::property_tree, RapidJSON and Google ProtoBuf included)
* Fast rendering (you can cache parsed templates and context objects)
* Optimized for speed (no regular expressions and few allocations)

Requirements
-----
* cmake
* C++14 compiler + STL (GCC, CLANG or MSVC 2015)
* BOOST (only header: lexical_cast, variant, optional, container)
* CATCH Unit Test Framework (shipped)
* C++17 string_view (shipped version is used if not available on your system)

Status
-----
This software has beta state but supports nearly all features of the original Ruby version of the Liquid Markup Language.

- on Travis-CI: [![Travis Build Status](https://travis-ci.org/mrpi/liquidpp.svg?branch=master)](https://travis-ci.org/mrpi/liquidpp)
- on AppVeyor: [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/91g3twect2bacw3w/branch/master?svg=true)](https://ci.appveyor.com/project/mrpi/liquidpp)
