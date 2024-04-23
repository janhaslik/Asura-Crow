cmake_minimum_required(VERSION 3.0)
project(Indexer)

set(CMAKE_CXX_STANDARD 17)

# Find the required packages using pkg-config
find_package(PkgConfig REQUIRED)

# Use pkg-config to locate and retrieve information about dependencies
pkg_check_modules(LIBBSONCXX REQUIRED libbsoncxx)
pkg_check_modules(LIBMONGOCXX REQUIRED libmongocxx)

# Include directories for the MongoDB C++ driver and its dependencies
include_directories(Indexer PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${LIBMONGOCXX_INCLUDE_DIRS}
    ${LIBBSONCXX_INCLUDE_DIRS}
)

# Add your source files to the executable
add_executable(Indexer 
    api/api.cpp 
    indexer/indexer.cpp 
    db/db.cpp
)

# Link your custom library and MongoDB C++ driver libraries
target_link_libraries(Indexer PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/libJetPlusPlusLib.dylib
    ${LIBMONGOCXX_LIBRARIES}
   ${LIBBSONCXX_LIBRARIES}
)

# Set compiler options for the target
target_compile_options(Indexer PRIVATE
      # ${LIBMONGOCXX_CFLAGS_OTHER}  # Additional flags for libmongocxx
   #     ${LIBBSONCXX_CFLAGS_OTHER}   # Additional flags for libbsoncxx
        -std=c++17
        -I/opt/homebrew/include/mongocxx/v_noabi
        -I/opt/homebrew/include/bsoncxx/v_noabi
        -L/opt/homebrew/lib
        -lmongocxx
        -lbsoncxx

)