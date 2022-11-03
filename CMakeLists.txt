cmake_minimum_required(VERSION 3.20.0)

# Project info
project(
    MgxParser
    VERSION 0.1.0 
    LANGUAGES CXX 
    HOMEPAGE_URL "aocrec.com" 
    DESCRIPTION "MgxParser is a C++ lib used to parse Age of Empires II game records."
)

# Some basic configurations
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_POSITION_INDEPENDENT_CODE true)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if (APPLE)
    # Mac hack, libiconv and libpng was install with brew
    include_directories(/opt/homebrew/opt/libpng/include)
    link_directories(/opt/homebrew/opt/libpng/lib)
    set(MGXPARSER_LINK_TARGETS pthread png z iconv)
elseif (WIN32)
    # windows with vcpkg
    include_directories(
        "D:\\vcpkg\\vcpkg\\packages\\zlib_x64-windows-static\\include" 
        "D:\\vcpkg\\vcpkg\\packages\\libiconv_x64-windows-static\\include" 
        "D:\\vcpkg\\vcpkg\\packages\\libpng_x64-windows-static\\include"
        "D:\\vcpkg\\vcpkg\\packages\\pthreads_x64-windows-static\\lib"
    )
    link_directories(
        "D:\\vcpkg\\vcpkg\\packages\\zlib_x64-windows-static\\lib" 
        "D:\\vcpkg\\vcpkg\\packages\\libiconv_x64-windows-static\\lib" 
        "D:\\vcpkg\\vcpkg\\packages\\libpng_x64-windows-static\\lib"
        "D:\\vcpkg\\vcpkg\\packages\\pthreads_x64-windows-static\\lib"
    )
    add_compile_definitions(_HAS_STD_BYTE=0)
    set(MGXPARSER_LINK_TARGETS pthreadvc3 libpng16 zlib iconv)
else()
    set(MGXPARSER_LINK_TARGETS pthread png z stdc++fs)
endif ()

# List source files to be compiled
aux_source_directory(./src SRC_ROOT)
aux_source_directory(src/tools TOOLS)
aux_source_directory(src/analyzers/default DEFAULT_ANALYZER)
aux_source_directory(src/analyzers/default/bodyProcessors DEFAULT_ANALYZER_BODY)
aux_source_directory(./libs/md5 MD5)
set(MGXPARSER_SRCS ${SRC_ROOT} ${DEFAULT_ANALYZER} ${DEFAULT_ANALYZER_BODY} ${TOOLS} ${MD5})

add_library("${PROJECT_NAME}_OBJECT" OBJECT ${MGXPARSER_SRCS})
add_library("${PROJECT_NAME}_SHARED" SHARED $<TARGET_OBJECTS:${PROJECT_NAME}_OBJECT>)
add_library("${PROJECT_NAME}_STATIC" STATIC $<TARGET_OBJECTS:${PROJECT_NAME}_OBJECT>)
add_executable("${PROJECT_NAME}_EXE" ${CMAKE_CURRENT_SOURCE_DIR}/test/maindemo.cpp)

# Import dependencies
# WARNING: png depends on libz, so put it before z
target_link_libraries("${PROJECT_NAME}_SHARED" ${MGXPARSER_LINK_TARGETS})
target_link_libraries("${PROJECT_NAME}_STATIC" ${MGXPARSER_LINK_TARGETS})
target_link_libraries("${PROJECT_NAME}_EXE" "${PROJECT_NAME}_STATIC" ${MGXPARSER_LINK_TARGETS})

set(
    MGXPARSER_INCLUDE
    PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/include/"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tools/"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/DataModels/" 
    "${PROJECT_BINARY_DIR}"
    )
target_include_directories("${PROJECT_NAME}_OBJECT" PRIVATE ${MGXPARSER_INCLUDE})
target_include_directories("${PROJECT_NAME}_SHARED" PRIVATE ${MGXPARSER_INCLUDE})
target_include_directories("${PROJECT_NAME}_STATIC" PRIVATE ${MGXPARSER_INCLUDE})
target_include_directories("${PROJECT_NAME}_EXE" PRIVATE ${MGXPARSER_INCLUDE})

# Import some environment variables and generate docs, etc.
string(TIMESTAMP COMPILEDATE "%Y%m%d")
target_compile_definitions("${PROJECT_NAME}_OBJECT" PRIVATE "DEBUG=$<CONFIG:Debug>")
configure_file(CompileConfig.h.in CompileConfig.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/README.md ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
# Update doxygen configuration file and generate output HTML
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})
if(BUILD_DOCUMENTATION AND NOT NODE_ADDON AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

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
endif()

# Testing configuration
# https://coderefinery.github.io/cmake-workshop/testing/
set(BUILD_TESTING 1)
if(BUILD_TESTING)
    set(CMP0135 1)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

	enable_testing()
    add_executable("${PROJECT_NAME}_TEST" ${CMAKE_CURRENT_SOURCE_DIR}/test/test.cpp)
    target_include_directories("${PROJECT_NAME}_TEST" PRIVATE ${MGXPARSER_INCLUDE})
    target_link_libraries("${PROJECT_NAME}_TEST" "${PROJECT_NAME}_STATIC" ${MGXPARSER_LINK_TARGETS})
    target_link_libraries("${PROJECT_NAME}_TEST" GTest::gtest_main)
    add_executable(
            "CURSOR_TEST"
            ${CMAKE_CURRENT_SOURCE_DIR}/test/cursor_test.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/src/cursor.cc
    )
    target_link_libraries("CURSOR_TEST" GTest::gtest_main iconv)
    include(GoogleTest)
    gtest_discover_tests("${PROJECT_NAME}_TEST")
    gtest_discover_tests("CURSOR_TEST")
endif()

# Static linking
if (NOT APPLE)
    target_link_options("${PROJECT_NAME}_EXE" PRIVATE "-static")
    target_link_options("${PROJECT_NAME}_TEST" PRIVATE "-static")
endif()