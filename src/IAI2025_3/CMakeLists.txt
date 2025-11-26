cmake_minimum_required(VERSION 3.10)

project(IAI2025_3 VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    main.cpp
    Dataset.cpp
    DecisionTree.cpp
    Example.cpp
    TreeNode.cpp
    util.cpp
)
add_executable(IAI2025_3 ${SOURCES})
target_include_directories(IAI2025_3 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Put generated executable into a convenient output directory
set_target_properties(IAI2025_3 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/output
)

# Convenience options for users building with Visual Studio
if(MSVC)
    message(STATUS "MSVC detected: building with Visual Studio generator")
endif()