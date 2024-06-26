cmake_minimum_required(VERSION 3.5)
project(Indexer)

set(CMAKE_CXX_STANDARD 17)

# Set the path to the mongo-cxx-driver source directory
set(MONGOCXX_DIR "${CMAKE_SOURCE_DIR}/mongo-cxx-driver")

# Specify the path to the mongocxx and bsoncxx include directories
set(LIBMONGOCXX_INCLUDE_DIRS "${MONGOCXX_DIR}/src/mongocxx/include")
set(LIBBSONCXX_INCLUDE_DIRS "${MONGOCXX_DIR}/src/bsoncxx/include")

# Add your source files to the executable
add_executable(Indexer 
    api/api.cpp 
    indexer/indexer.cpp 
    db/db.cpp
)

# Set include directories
target_include_directories(Indexer PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${LIBMONGOCXX_INCLUDE_DIRS}
    ${LIBBSONCXX_INCLUDE_DIRS}
)

# Link MongoDB C++ driver libraries
find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)

# Link libraries via imported targets
target_link_libraries(Indexer PRIVATE
    mongo::mongocxx_shared
    mongo::bsoncxx_shared
)

# Link additional libraries (assuming libJetPlusPlusLib.dylib)
target_link_libraries(Indexer PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/libJetPlusPlusLib.dylib
)