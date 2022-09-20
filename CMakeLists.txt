cmake_minimum_required(VERSION 3.0.0)
project(mgxParser VERSION 0.1.0)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

aux_source_directory(. SRC_ROOT)
add_executable(mgxParser ${SRC_ROOT})
target_link_libraries(mgxParser z)
target_link_libraries(mgxParser png)
target_link_libraries(mgxParser pthread)

configure_file(CompileConfig.h.in CompileConfig.h)

target_include_directories(mgxParser PUBLIC "${PROJECT_BINARY_DIR}")