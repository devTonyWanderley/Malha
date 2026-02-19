#	--IO--

project(IO LANGUAGES CXX)

add_library(IO STATIC
    arquivo.h
)

target_include_directories(IO PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
