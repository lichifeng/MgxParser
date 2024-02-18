cmake_minimum_required(VERSION 3.20.0)

# This configuration is for linux only

# Project info
project(
        MgxParser
        VERSION 0.4.6
        LANGUAGES CXX
        HOMEPAGE_URL "https://github.com/lichifeng/MgxParser"
        DESCRIPTION "MgxParser is a C++ lib used to parse Age of Empires II game records."
)
message("Build type: ${CMAKE_BUILD_TYPE}")

# Some basic configurations
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_POSITION_INDEPENDENT_CODE true)
set(USE_BM_SEARCH 1)

# Prepare source files to be compiled
aux_source_directory(./src SRC_ROOT)
aux_source_directory(src/tools TOOLS)
aux_source_directory(src/analyzers/default DEFAULT_ANALYZER)
aux_source_directory(src/analyzers/default/body_processors DEFAULT_ANALYZER_BODY)
aux_source_directory(./libs/md5 MD5)
set(MGXPARSER_SRCS ${DEFAULT_ANALYZER} ${DEFAULT_ANALYZER_BODY} ${MD5} ${SRC_ROOT} ${TOOLS})

# Prepare dependency libraries
# WARNING: png depends on libz, so put it before z
set(MGXPARSER_DEPENDENCIES ssl crypto png z)

# Create static and shared libraries
add_library("${PROJECT_NAME}_OBJECT" OBJECT ${MGXPARSER_SRCS})
add_library("${PROJECT_NAME}_SHARED" SHARED $<TARGET_OBJECTS:${PROJECT_NAME}_OBJECT>)
add_library("${PROJECT_NAME}_STATIC" STATIC $<TARGET_OBJECTS:${PROJECT_NAME}_OBJECT>)
target_link_libraries("${PROJECT_NAME}_SHARED" ${MGXPARSER_DEPENDENCIES})
target_link_libraries("${PROJECT_NAME}_STATIC" ${MGXPARSER_DEPENDENCIES})

# Create static and shared executables
add_executable("${PROJECT_NAME}_S_EXE" demo.cc)
target_link_libraries("${PROJECT_NAME}_S_EXE" ${PROJECT_NAME}_STATIC)
target_link_options("${PROJECT_NAME}_S_EXE" PRIVATE "-static")
add_executable("${PROJECT_NAME}_D_EXE" demo.cc)
target_link_libraries("${PROJECT_NAME}_D_EXE" ${PROJECT_NAME}_SHARED)
set_target_properties(${PROJECT_NAME}_D_EXE PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "$ORIGIN"
)

set(
        MGXPARSER_INCLUDE
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/include/"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/tools/"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/datamodels/"
        "${PROJECT_BINARY_DIR}" 
)
target_include_directories("${PROJECT_NAME}_OBJECT" PRIVATE ${MGXPARSER_INCLUDE})
target_include_directories("${PROJECT_NAME}_D_EXE" PRIVATE ${MGXPARSER_INCLUDE})
target_include_directories("${PROJECT_NAME}_S_EXE" PRIVATE ${MGXPARSER_INCLUDE})

# Compile Node.js Addon
add_definitions(-DNAPI_VERSION=3)
# 检查napi.h是否存在于CMAKE_JS_INC目录中
find_path(NAPI_H_FOUND "node_api.h" PATHS ${CMAKE_JS_INC})

# 如果napi.h和CMAKE_JS_LIB都存在，创建mgxnodeS库
if(NAPI_H_FOUND)
        # Include Node-API wrappers
        execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
        string(REGEX REPLACE "[\r\n\"]" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

        set(NODE_ADDON_NAME "mgxnode")
        add_library(${NODE_ADDON_NAME} SHARED $<TARGET_OBJECTS:${PROJECT_NAME}_OBJECT> mgxnode.cc)
        set_target_properties(${NODE_ADDON_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
        target_link_libraries(${NODE_ADDON_NAME} ${CMAKE_JS_LIB} ${MGXPARSER_DEPENDENCIES})
        target_include_directories(${NODE_ADDON_NAME} PRIVATE ${MGXPARSER_INCLUDE} ${CMAKE_JS_INC} ${NODE_ADDON_API_DIR})
else()
        message("cmake-js not found, mgxnode library will not be created.")
endif()

# Import some environment variables and generate docs, etc.
string(TIMESTAMP COMPILEDATE "%Y%m%d")
target_compile_definitions("${PROJECT_NAME}_OBJECT" PRIVATE "DEBUG=$<CONFIG:Debug>")
configure_file(compile_config.h.in compile_config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/README.md ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

# Update doxygen configuration file and generate output HTML
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})
if (BUILD_DOCUMENTATION AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
    if (NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif ()

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/docs/template/Doxyfile.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # copy the content from `Doxyfile.in` to `Doxyfile`, replace the @VAR@ variables
    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    add_custom_target(
            doc ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating documentation with Doxygen"
            VERBATIM)

    #install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs-html DESTINATION share)
endif ()

# Testing configuration
# https://coderefinery.github.io/cmake-workshop/testing/
set(BUILD_TESTING 0)
if (BUILD_TESTING)
    set(CMP0135 1)
    set(DOWNLOAD_EXTRACT_TIMESTAMP 1)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_executable(
            "${PROJECT_NAME}_TEST"
            ${CMAKE_CURRENT_SOURCE_DIR}/test/test.cc
    )
    target_include_directories("${PROJECT_NAME}_TEST" PRIVATE ${MGXPARSER_INCLUDE})
    target_link_libraries("${PROJECT_NAME}_TEST" ${PROJECT_NAME}_STATIC)
    target_link_libraries("${PROJECT_NAME}_TEST" GTest::gtest_main)
    add_executable(
            "CURSOR_TEST"
            ${CMAKE_CURRENT_SOURCE_DIR}/test/cursor_test.cc
            src/tools/cursor.cc
    )
    target_link_libraries("CURSOR_TEST" ${PROJECT_NAME}_STATIC)
    target_link_libraries("CURSOR_TEST" GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests("${PROJECT_NAME}_TEST")
    gtest_discover_tests("CURSOR_TEST")
endif ()