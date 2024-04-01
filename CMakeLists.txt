cmake_minimum_required(VERSION 3.0)
project(sylar)

include (cmake/utils.cmake)
add_compile_options(-Wno-builtin-macro-redefined)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS}  -O0 -ggdb -std=c++17 -Wall -Wno-deprecated -Werror -Wno-unused-function ")

include_directories(.)
include_directories(/usr/local/include)
include_directories(/home/ts/project/sylar-ts/sylar)
link_directories(/usr/local/lib)

find_library(YAMLCPP yaml-cpp)


set(LIB_SRC
    sylar/log.cc
    sylar/util.cc
    sylar/config.cc
    sylar/mythread.cc
    )

add_library(sylar SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(sylar)   #__FILE__
#add_library(sylar_static STATIC ${LIB_SRC})
#SET_TARGET_PROPERTIES (sylar_static PROPERTIES OUTPUT_NAME "sylar")

set(LIB_LIB sylar 
            pthread
            ${YAMLCPP})

add_executable(test_log tests/test_log.cc)
add_dependencies(test_log sylar)
force_redefine_file_macro_for_sources(test_log)   #__FILE__
target_link_libraries(test_log ${LIB_LIB})

add_executable(test_config tests/test_config.cc)
add_dependencies(test_config sylar)
force_redefine_file_macro_for_sources(test_config)   #__FILE__
target_link_libraries(test_config ${LIB_LIB})

add_executable(test_thread tests/test_thread.cc)
add_dependencies(test_thread sylar)
force_redefine_file_macro_for_sources(test_thread)   #__FILE__
target_link_libraries(test_thread ${LIB_LIB})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)