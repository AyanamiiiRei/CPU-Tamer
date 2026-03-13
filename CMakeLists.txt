cmake_minimum_required(VERSION 3.10)
project(CPU-Tamer C)

# 设置 C 标准
set(CMAKE_C_STANDARD 11)

# 【关键】告诉 CLion 头文件在 include 目录下
# ${CMAKE_SOURCE_DIR} 代表 CMakeLists.txt 所在的根目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 收集 src 目录下的所有 .c 文件
file(GLOB SOURCES "src/*.c")

# 创建可执行文件
add_executable(cputamer ${SOURCES})