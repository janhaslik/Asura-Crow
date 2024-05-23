cmake_minimum_required(VERSION 3.10)
project(Search)

set(CMAKE_CXX_STANDARD 17)

# Set the path to the mongo-cxx-driver source directory
set(MONGOCXX_DIR "${CMAKE_SOURCE_DIR}/mongo-cxx-driver")

# Specify the path to the mongocxx and bsoncxx include directories
set(LIBMONGOCXX_INCLUDE_DIRS "${MONGOCXX_DIR}/src/mongocxx/include")
set(LIBBSONCXX_INCLUDE_DIRS "${MONGOCXX_DIR}/src/bsoncxx/include")

list(APPEND CMAKE_PREFIX_PATH "${MONGOCXX_DIR}/buildcmake")

add_executable(Search api/api.cpp searcher/searcher.cpp db/db.cpp)

# Include directory for the library headers
include_directories(
    ${CMAKE_SOURCE_DIR}/include     
    ${LIBMONGOCXX_INCLUDE_DIRS}
    ${LIBBSONCXX_INCLUDE_DIRS})

# Link MongoDB C++ driver libraries
find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)

# Link libraries via imported targets
target_link_libraries(Search PRIVATE
    mongo::mongocxx_shared
    mongo::bsoncxx_shared
)

# Specify the relative path to your library without -l flag
target_link_libraries(Search PRIVATE ${CMAKE_SOURCE_DIR}/lib/libJetPlusPlusLib.dylib)
