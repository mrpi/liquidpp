
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/liquidpp/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/liquidpp/config.h)

add_library (liquidpp STATIC
            liquidpp/parser.cpp liquidpp/parser.hpp
            liquidpp/template.hpp )
