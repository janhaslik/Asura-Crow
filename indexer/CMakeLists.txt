cmake_minimum_required(VERSION 3.0)
project(Indexer)

set(CMAKE_CXX_STANDARD 17)

# Add your source files to the executable
add_executable(Indexer 
    api/api.cpp 
    indexer/indexer.cpp 
    db/db.cpp
)

# Set include directories
target_include_directories(Indexer PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Find and link SQLite3
find_package(SQLite3 REQUIRED)

target_link_libraries(Indexer PRIVATE SQLite::SQLite3)
target_link_libraries(Indexer PRIVATE ${CMAKE_SOURCE_DIR}/lib/libJetPlusPlusLib.dylib)
