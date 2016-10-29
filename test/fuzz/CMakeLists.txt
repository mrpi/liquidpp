
add_executable (liquidppFuzz
        liquidppFuzz.cpp)
set_target_properties(liquidppFuzz PROPERTIES EXCLUDE_FROM_ALL TRUE)

target_link_libraries (liquidppFuzz
        liquidpp
        /usr/lib/llvm-3.9/lib/libFuzzer.a
        pthread)
