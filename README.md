# liquidpp
C++14 version of the [Liquid Markup Language](https://shopify.github.io/liquid/)

Usage
-----

```C++
liquidpp::Context c;
c.set("name", "Donald Drumpf");
auto rendered = liquidpp::parse("Hello {{name}}!")(c);
REQUIRE("Hello Donald Drumpf!" == rendered);
```

Features
-----
* Buildin support for multiple stl and boost types
* Extendable with user types

Requirements
-----
* cmake
* C++14 compiler
* STL and boost (lexical_cast, spirit, variant, optional)
* CATCH Unit Test Framework (shipped)

Status
-----
This software is an early alpha state, has only limited test coverage and misses many features of the original Liquid Markup Language.

- on Travis-CI: [![Travis Build Status](https://travis-ci.org/mrpi/liquidpp.svg?branch=master)](https://travis-ci.org/mrpi/liquidpp)
