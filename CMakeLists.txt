# =============================================================================
# CMakeLists.txt - ICPC Management System (2026)
#
# 本文件服务于两种用法：
#
# 1) ACM Class OnlineJudge 的「Git 编译流程」（原地构建 in-source build）
#        cmake .
#        make
#    评测机会把仓库根目录的 code 文件作为编译结果，因此可执行文件的输出
#    目录被显式固定为 ${CMAKE_BINARY_DIR}（原地构建时即仓库根目录）。
#
# 2) 本地开发（out-of-source build，不污染工作区）
#        cmake -S . -B build && cmake --build build -j
#    此时可执行文件为 build/code。
#
# 编译参数与 build.sh 保持一致：-std=c++20 -O2 -Wall -Wextra。
# =============================================================================

cmake_minimum_required(VERSION 3.10)

project(ICPC-Management-System-2026 LANGUAGES CXX)

# -----------------------------------------------------------------------------
# 语言标准：与 OJ 的 C++ 编译环境（g++ -std=c++20）保持一致
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------------------------------------------------------
# 源文件：src/ 下所有 .cpp 各作为一个翻译单元（不需要 merge.sh 合并）
# -----------------------------------------------------------------------------
file(GLOB ICPC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

if(NOT ICPC_SOURCES)
    message(FATAL_ERROR "未在 src/ 下找到任何 .cpp 源文件，请检查仓库结构。")
endif()

add_executable(code ${ICPC_SOURCES})

# src/ 下的头文件（forward.h / Log.hpp）以 #include "..." 形式包含
target_include_directories(code PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# -----------------------------------------------------------------------------
# 输出位置：可执行文件必须名为 code，且原地构建时落在项目根目录
# -----------------------------------------------------------------------------
set_target_properties(code PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# 多配置生成器（Ninja Multi-Config / Visual Studio）需要逐配置指定
get_property(ICPC_CONFIGS GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(ICPC_CONFIGS OR CMAKE_CONFIGURATION_TYPES)
    foreach(cfg ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER "${cfg}" cfg_upper)
        set_target_properties(code PROPERTIES
            "RUNTIME_OUTPUT_DIRECTORY_${cfg_upper}" "${CMAKE_BINARY_DIR}"
        )
    endforeach()
endif()

# -----------------------------------------------------------------------------
# 编译选项
# 默认（评测机的 `cmake .`，不指定构建类型）与 build.sh 一致：-O2 -Wall -Wextra；
# 显式指定 CMAKE_BUILD_TYPE 时只加告警选项，优化级别交给 CMake 决定。
# 注意：此处不定义 NDEBUG —— 代码中的 assert 用于校验输入合法性，需保留其语义。
# -----------------------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(code PRIVATE -Wall -Wextra)
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        target_compile_options(code PRIVATE -O2)
    endif()
elseif(MSVC)
    target_compile_options(code PRIVATE /W3 /utf-8)
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        target_compile_options(code PRIVATE /O2)
    endif()
endif()
