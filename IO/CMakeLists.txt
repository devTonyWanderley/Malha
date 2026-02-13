project(IO LANGUAGES CXX)

add_library(IO SHARED 
    arquivo.h 
    arquivo.cpp
)

target_include_directories(IO PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
