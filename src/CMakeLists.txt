
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/liquidpp/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/liquidpp/config.h)

add_library (liquidpp STATIC
            liquidpp/parser.cpp liquidpp/parser.hpp
        liquidpp/Template.hpp liquidpp/Template.cpp liquidpp/tags/If.hpp liquidpp/tags/If.cpp liquidpp/tags/Tag.hpp liquidpp/tags/UnevaluatedTag.hpp liquidpp/tags/Block.hpp liquidpp/BlockBody.hpp liquidpp/Variable.hpp liquidpp/tags/Comment.hpp liquidpp/tags/For.hpp liquidpp/tags/For.cpp liquidpp/TagFactory.hpp liquidpp/TagFactory.cpp liquidpp/BlockBody.cpp liquidpp/Variable.cpp)
