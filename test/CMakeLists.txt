include_directories (${liquidpp_SOURCE_DIR}/src
                     ${liquidpp_SOURCE_DIR}/external
                     ${Boost_INCLUDE_DIRS}
                     )

file(DOWNLOAD https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp ${liquidpp_SOURCE_DIR}/external/catch.hpp)

if (PROTOBUF_FOUND)
   include_directories(${CMAKE_CURRENT_BINARY_DIR})
   include_directories(${PROTOBUF_INCLUDE_DIRS})
   PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS addressbook.proto)
endif (PROTOBUF_FOUND)

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
        filters.cpp
        tag_capture.cpp
        whitespace_control.cpp
        control_flow.cpp
        iteration.cpp
        forloop_object.cpp
        tag_increment.cpp
        tag_cycle.cpp
        multiple_error_cases.cpp
        ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries (liquidppTest
                       liquidpp
                       ${PROTOBUF_LIBRARIES})

add_test(NAME liquidppTest COMMAND liquidppTest)

add_subdirectory(fuzz)

add_subdirectory(try_online)
