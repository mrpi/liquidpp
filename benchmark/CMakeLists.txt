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
set_target_properties(liquidppBenchmark PROPERTIES EXCLUDE_FROM_ALL TRUE)

target_link_libraries (liquidppBenchmark
                       liquidpp
                       pthread)

add_executable (benchmarkFiles
        benchmarkFiles.cpp)

target_link_libraries (benchmarkFiles
        liquidpp)
