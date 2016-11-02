
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/liquidpp/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/liquidpp/config.h)

add_library (liquidpp STATIC
        liquidpp/parser.hpp
        liquidpp/Template.cpp liquidpp/Template.hpp
        liquidpp/Key.cpp liquidpp/Key.hpp
        liquidpp/BlockBody.cpp liquidpp/BlockBody.hpp
        liquidpp/Variable.cpp liquidpp/Variable.hpp
        liquidpp/TagFactory.cpp liquidpp/TagFactory.hpp
        liquidpp/Expression.hpp liquidpp/Expression.cpp
        liquidpp/FilterFactory.hpp liquidpp/FilterFactory.cpp        
        liquidpp/Exception.hpp
        liquidpp/tags/Comment.hpp
        liquidpp/tags/For.cpp liquidpp/tags/For.hpp
        liquidpp/tags/Conditional.hpp
        liquidpp/tags/UnevaluatedTag.hpp
        liquidpp/tags/Tag.cpp liquidpp/tags/Tag.hpp
        liquidpp/tags/Block.hpp
        liquidpp/tags/Capture.hpp liquidpp/tags/Capture.cpp
        liquidpp/tags/Case.hpp liquidpp/tags/Case.cpp
        liquidpp/tags/Increment.hpp liquidpp/tags/Cycle.hpp 
        liquidpp/tags/Assign.hpp liquidpp/tags/Assign.cpp
        liquidpp/filters/Filter.hpp
        liquidpp/filters/Escape.hpp
        liquidpp/filters/Size.hpp
        liquidpp/filters/Downcase.hpp
        liquidpp/filters/Upcase.hpp
        liquidpp/filters/Capitalize.hpp
        liquidpp/filters/Assign.hpp
        liquidpp/filters/NumberFilter.hpp
        liquidpp/filters/Default.hpp
        liquidpp/filters/FloatFilter.hpp
        liquidpp/filters/Lstrip.hpp
        liquidpp/filters/NewlineToBr.hpp 
        liquidpp/filters/Prepend.hpp
        liquidpp/filters/Remove.hpp
        liquidpp/filters/RemoveFirst.hpp
        liquidpp/filters/Replace.hpp
        liquidpp/filters/ReplaceFirst.hpp
        liquidpp/filters/Round.hpp
        liquidpp/filters/Rstrip.hpp
        liquidpp/filters/Slice.hpp
        liquidpp/filters/Strip.hpp
        liquidpp/filters/StripNewlines.hpp
        liquidpp/filters/TruncateWords.hpp 
        liquidpp/filters/Truncate.hpp
        liquidpp/filters/Date.hpp 
        liquidpp/filters/StripHtml.hpp 
        liquidpp/filters/UrlEncode.hpp 
        liquidpp/filters/Split.hpp 
        liquidpp/filters/Join.hpp 
        liquidpp/filters/Sort.hpp 
        liquidpp/filters/Reverse.hpp 
        liquidpp/filters/Uniq.hpp 
        liquidpp/filters/Map.hpp)
