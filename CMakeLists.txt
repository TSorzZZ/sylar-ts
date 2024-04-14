cmake_minimum_required(VERSION 3.0) # 指定CMake的最低版本要求为3.0
project(sylar) # 定义项目名称为sylar

include (cmake/utils.cmake) # 包含一个名为utils.cmake的文件，可能包含一些通用的CMake函数或变量
add_compile_options(-Wno-builtin-macro-redefined) # 添加编译选项，禁止警告关于重定义内置宏
set(CMAKE_VERBOSE_MAKEFILE ON) # 设置CMake生成的Makefile为详细模式，方便调试
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 导出编译命令，用于IDE或其他工具使用
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -ggdb -std=c++17 -Wall -Wno-deprecated -Werror -Wno-unused-function ") # 设置C++编译器的标志，包括环境变量中的CXXFLAGS、优化级别、调试信息、C++标准、警告控制等

include_directories(.) # 添加当前目录到头文件搜索路径
include_directories(/usr/local/include) # 添加/usr/local/include到头文件搜索路径
include_directories(/home/ts/project/sylar-ts/sylar) # 添加/home/ts/project/sylar-ts/sylar到头文件搜索路径
link_directories(/usr/local/lib) # 添加/usr/local/lib到库文件搜索路径

find_library(YAMLCPP yaml-cpp) # 查找名为yaml-cpp的库，并将其路径存储在变量YAMLCPP中

set(LIB_SRC 
    sylar/log.cc
    sylar/util.cc
    sylar/config.cc
    sylar/mythread.cc
    )

add_library(sylar SHARED ${LIB_SRC}) # 添加一个名为sylar的共享库，源文件来自LIB_SRC变量
force_redefine_file_macro_for_sources(sylar) 
#add_library(sylar_static STATIC ${LIB_SRC}) # 注释掉的代码，用于添加一个名为sylar_static的静态库
#SET_TARGET_PROPERTIES (sylar_static PROPERTIES OUTPUT_NAME "sylar") 

set(LIB_LIB sylar 
            pthread
            ${YAMLCPP})

add_executable(test_log tests/test_log.cc) # 添加一个可执行文件test_log，源文件为tests/test_log.cc
add_dependencies(test_log sylar) # 设置test_log依赖于sylar库
force_redefine_file_macro_for_sources(test_log) 
target_link_libraries(test_log ${LIB_LIB}) # 链接test_log可执行文件到LIB_LIB中的库

add_executable(test_config tests/test_config.cc) # 添加一个可执行文件test_config，源文件为tests/test_config.cc
add_dependencies(test_config sylar) # 设置test_config依赖于sylar库
force_redefine_file_macro_for_sources(test_config) 
target_link_libraries(test_config ${LIB_LIB}) # 链接test_config可执行文件到LIB_LIB中的库

add_executable(test_thread tests/test_thread.cc) # 添加一个可执行文件test_thread，源文件为tests/test_thread.cc
add_dependencies(test_thread sylar) # 设置test_thread依赖于sylar库
force_redefine_file_macro_for_sources(test_thread) 
target_link_libraries(test_thread ${LIB_LIB}) # 链接test_thread可执行文件到LIB_LIB中的库

add_executable(test_util tests/test_util.cc) # 添加一个可执行文件test_util，源文件为tests/test_util.cc
add_dependencies(test_util sylar) # 设置test_util依赖于sylar库
force_redefine_file_macro_for_sources(test_util)
target_link_libraries(test_util ${LIB_LIB}) # 链接test_util可执行文件到LIB_LIB中的库

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) # 设置可执行文件的输出路径为项目源目录下的bin目录
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib) # 设置库文件的输出路径为项目源目录下的lib目录