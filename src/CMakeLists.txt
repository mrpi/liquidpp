
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/liquidpp/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/liquidpp/config.h)

add_library (liquidpp STATIC
        liquidpp/parser.cpp liquidpp/parser.hpp
        liquidpp/Template.cpp liquidpp/Template.hpp
        liquidpp/BlockBody.cpp liquidpp/BlockBody.hpp
        liquidpp/Variable.cpp liquidpp/Variable.hpp
        liquidpp/TagFactory.cpp liquidpp/TagFactory.hpp
        liquidpp/tags/Comment.hpp
        liquidpp/tags/For.cpp liquidpp/tags/For.hpp
        liquidpp/tags/If.cpp liquidpp/tags/If.hpp
        liquidpp/tags/UnevaluatedTag.hpp
        liquidpp/tags/Tag.cpp liquidpp/tags/Tag.hpp
        liquidpp/tags/Block.hpp)
