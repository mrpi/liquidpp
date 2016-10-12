include(CheckCXXCompilerFlag)

include_directories (${liquidpp_SOURCE_DIR}/src
                     ${Boost_INCLUDE_DIRS}
                     )

CHECK_CXX_COMPILER_FLAG("-fext-numeric-literals" COMPILER_SUPPORTS_EXT_NUM_LIT)                     
                     
if(COMPILER_SUPPORTS_EXT_NUM_LIT)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fext-numeric-literals")
endif(COMPILER_SUPPORTS_EXT_NUM_LIT)

add_executable (liquidppBenchmark
                benchMain.cpp)                

target_link_libraries (liquidppBenchmark
                       liquidpp
                       pthread)
