cmake_minimum_required(VERSION 3.0) # 指定CMake的最低版本要求为3.0
project(sylar) # 定义项目名称为sylar

include (cmake/utils.cmake) # 包含一个名为utils.cmake的文件，可能包含一些通用的CMake函数或变量
add_compile_options(-Wno-builtin-macro-redefined) # 添加编译选项，禁止警告关于重定义内置宏
set(CMAKE_VERBOSE_MAKEFILE ON) # 设置CMake生成的Makefile为详细模式，方便调试
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 导出编译命令，用于IDE或其他工具使用
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -ggdb -std=c++17 -Wall -Wno-deprecated ") # 设置C++编译器的标志，包括环境变量中的CXXFLAGS、优化级别、调试信息、C++标准、警告控制等

# 指定头文件搜索路径
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/sylar/include/websocket) 
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/sylar/include/http) 
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/sylar/include) 
include_directories(.) 
# 指定库文件搜索路径
link_directories(/usr/local/lib) 

# 查找名为yaml-cpp的库，并将其路径存储在变量YAMLCPP中
find_library(YAMLCPP yaml-cpp) 
find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
endif()

# 递归查找所有源文件
file(GLOB_RECURSE SRC_DIR_LIST ${PROJECT_SOURCE_DIR}/sylar/src/*.cc 
                               ${PROJECT_SOURCE_DIR}/sylar/src/*.cpp 
                               ${PROJECT_SOURCE_DIR}/sylar/src/http/*.cc
                               ${PROJECT_SOURCE_DIR}/sylar/src/websocket/*.cc)    

# 调用 ragelmaker 函数来生成 .rl.cc 文件
ragelmaker(sylar/include/http/http11_parser.rl SRC_DIR_LIST ${PROJECT_SOURCE_DIR}/sylar/src/http)
ragelmaker(sylar/include/http/httpclient_parser.rl SRC_DIR_LIST ${PROJECT_SOURCE_DIR}/sylar/src/http)
ragelmaker(sylar/include/uri.rl SRC_DIR_LIST ${PROJECT_SOURCE_DIR}/sylar/src)

# 创建一个名为sylar的动态库 
add_library(sylar SHARED ${SRC_DIR_LIST}) 
# add_library(sylar STATIC ${SRC_DIR_LIST}) 

set(LIB_LIB sylar pthread dl ${YAMLCPP} ${OPENSSL_LIBRARIES})

# 递归查找所有test源文件
file(GLOB_RECURSE TEST_DIR_LIST ${PROJECT_SOURCE_DIR}/tests/*.cc 
                                ${PROJECT_SOURCE_DIR}/tests/*.cpp 
                                ${PROJECT_SOURCE_DIR}/examples/*.cc
                                ${PROJECT_SOURCE_DIR}/samples/*.cc)    

foreach(TEST ${TEST_DIR_LIST})
    # 从原文件中提取目标名称 Name Without Extension
    get_filename_component(TARGET_NAME ${TEST} NAME_WE)
    # 添加可执行测试用例文件
    add_executable(${TARGET_NAME} ${TEST})
    # 指定依赖的库文件
    add_dependencies(${TARGET_NAME} sylar)
    force_redefine_file_macro_for_sources(${TARGET_NAME})
    # 链接动态库  要放在添加可执行文件后
    target_link_libraries(${TARGET_NAME} ${LIB_LIB})
endforeach()


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) # 设置可执行文件的输出路径为项目源目录下的bin目录
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib) # 设置库文件的输出路径为项目源目录下的lib目录

