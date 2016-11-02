
add_executable (liquidppTryOnline
        liquidppTryOnline.cpp)
#set_target_properties(liquidppTryOnline PROPERTIES EXCLUDE_FROM_ALL TRUE)

target_link_libraries (liquidppTryOnline
        liquidpp
        pthread
        boost_system
        cppnetlib-server-parsers)

