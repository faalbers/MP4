cmake_minimum_required(VERSION 3.4)
project(MP4 VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(MP4_PARSE_PATH)
#add_compile_definitions(MP4_PARSE_DATA)

file(GLOB ATOMS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} atoms/*.cpp )
add_library(MP4 MP4.cpp ${ATOMS})
target_include_directories(MP4 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(testMP4 testMP4.cpp)
target_link_libraries(testMP4 PRIVATE MP4)
