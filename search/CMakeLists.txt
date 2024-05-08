cmake_minimum_required(VERSION 3.10)
project(Search)

set(CMAKE_CXX_STANDARD 17)

# Include directory for the library headers
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(Search api/api.cpp searcher/searcher.cpp)

# Specify the relative path to your library without -l flag
target_link_libraries(Search PRIVATE ${CMAKE_SOURCE_DIR}/lib/libJetPlusPlusLib.dylib)