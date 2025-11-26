cmake_minimum_required(VERSION 3.10)  # 确保 CMake 版本支持这些命令
project(IAI2025_2)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)  # 关键修改：将标准设置为 14 或更高
set(CMAKE_CXX_STANDARD_REQUIRED ON)    # 确保编译器严格使用指定标准，如果不行则报错

# 调试配置
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        # MSVC 编译器选项
        # /Zi: 生成调试信息
        # /Od: 关闭优化
        # /W4: 启用高级别警告
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi /Od /W4")
    else()
        # GCC/Clang 编译器选项
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Wextra")
    endif()
    message(STATUS "Building in Debug mode")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    message(STATUS "Building in Release mode")
endif()

# 如果使用 OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# 包含头文件目录
include_directories(
    ${CMAKE_SOURCE_DIR}/../../indicators/include
)

# 查找所有源文件
file(GLOB SOURCE_FILES "*.cpp")

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# 链接 OpenMP（如果使用）
if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_CXX)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    OBJECT_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/obj"
)
