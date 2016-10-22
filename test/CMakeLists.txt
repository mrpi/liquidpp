include_directories (${liquidpp_SOURCE_DIR}/src
                     ${liquidpp_SOURCE_DIR}/external
                     ${Boost_INCLUDE_DIRS}
                     )

file(DOWNLOAD https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp ${liquidpp_SOURCE_DIR}/external/catch.hpp)

add_executable (liquidppTest
        main.cpp
        block_unit_test.cpp
        path_and_keys.cpp
        render.cpp
        ranges.cpp
        tag_for.cpp
        tag_if.cpp
        tokenize.cpp
        condition_unit_test.cpp
        tag_assign.cpp
        filters.cpp tag_capture.cpp whitespace_control.cpp control_flow.cpp iteration.cpp forloop_object.cpp)


target_link_libraries (liquidppTest
                       liquidpp)
                       
add_test(NAME liquidppTest COMMAND liquidppTest)
