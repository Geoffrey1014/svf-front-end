cmake_minimum_required(VERSION 3.24)
project(SVF_Frontend
        VERSION "0.1.0"
        DESCRIPTION "C++ frontend for the SVF"
        LANGUAGES C CXX
)

# set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")


message(STATUS "Building ${PROJECT_NAME} with configuration:
  SVF_Frontend version:                         ${SVF_Frontend_VERSION}
  SVF_Frontend root directory:                           ${SVF_Frontend_SOURCE_DIR}
  SVF_Frontend binary directory:                         ${SVF_Frontend_BINARY_DIR}"
)

message(STATUS "Using CMake build configuration:
  CMake generator:                              ${CMAKE_GENERATOR}
  CMake C compiler:                             ${CMAKE_C_COMPILER_ID}
  CMake C++ compiler:                           ${CMAKE_CXX_COMPILER_ID}
  CMake current list directory:                 ${CMAKE_CURRENT_LIST_DIR}
  CMake current source directory:               ${CMAKE_CURRENT_SOURCE_DIR}
  CMake current binary directory:               ${CMAKE_CURRENT_BINARY_DIR}"
)

if (EXISTS "${Tree_Sitter_DIR}")
else()
    set(Tree_Sitter_DIR $ENV{Tree_Sitter_DIR})
    if(EXISTS "${Tree_Sitter_DIR}")
    else()
    message(FATAL_ERROR "\
WARNING: The Tree_Sitter_DIR var was not set (required for an out-of-source build)!\n\
Please set this to environment variable to point to the Tree_Sitter_DIR directory or set this variable to cmake configuration\n
(e.g. on linux: export Tree_Sitter_DIR=/path/to/tree-sitter/dir) \n or \n \n(make the project via: cmake -DTree_Sitter_DIR=your_path_to_Tree_Sitter) ")
    endif()
endif() 


set(Tree_Sitter_HEADER "${Tree_Sitter_DIR}/lib/include")
set(Tree_Sitter_LIB "${Tree_Sitter_DIR}/libtree-sitter.a")


include_directories(
  ${Tree_Sitter_HEADER}
  include 
  argparse/include)

# Automatically add all .cpp files in the src directory to the SOURCES list
file(GLOB SOURCES "src/*.cpp" "tree-sitter-c/src/parser.c")

add_executable(svf_frontend ${SOURCES})


target_link_libraries(svf_frontend ${Tree_Sitter_LIB})

# Find Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Create test executable
add_executable(test_ssa test/TestSSA.cpp)

# Link against Google Test and pthread
target_link_libraries(test_ssa ${GTEST_LIBRARIES} pthread)
