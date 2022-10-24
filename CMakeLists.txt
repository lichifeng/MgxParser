cmake_minimum_required(VERSION 3.0.0)

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

# List source files to be compiled
aux_source_directory(./src SRC_ROOT)
aux_source_directory(./src/DataModels DATAMODELS)
aux_source_directory(./src/Analyzers/default DEFAULT_ANALYZER)
aux_source_directory(./src/Analyzers/default/bodyProcessors DEFAULT_ANALYZER_BODY)
aux_source_directory(./libs/md5 MD5)
set(MGXPARSER_SRCS ${SRC_ROOT} ${DEFAULT_ANALYZER} ${DEFAULT_ANALYZER_BODY} ${DATAMODELS} ${MD5})

add_library("${PROJECT_NAME}_OBJECT" OBJECT ${MGXPARSER_SRCS})
add_library("${PROJECT_NAME}_SHARED" SHARED $<TARGET_OBJECTS:${PROJECT_NAME}_OBJECT>)
add_library("${PROJECT_NAME}_STATIC" STATIC $<TARGET_OBJECTS:${PROJECT_NAME}_OBJECT>)
add_executable("${PROJECT_NAME}_EXE" ./test/maindemo.cpp)
target_link_options("${PROJECT_NAME}_EXE" PRIVATE "-static")

# Import dependencies
# WARNING: png depends on libz, so put it before z
set(MGXPARSER_LINK_TARGETS pthread png z "stdc++fs")
target_link_libraries("${PROJECT_NAME}_SHARED" ${MGXPARSER_LINK_TARGETS})
target_link_libraries("${PROJECT_NAME}_STATIC" ${MGXPARSER_LINK_TARGETS})
target_link_libraries("${PROJECT_NAME}_EXE" "${PROJECT_NAME}_STATIC" ${MGXPARSER_LINK_TARGETS})

set(
    MGXPARSER_INCLUDE
    PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/include/" 
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
if(BUILD_DOCUMENTATION AND NOT NODE_ADDON)
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
if(BUILD_TESTING)
	enable_testing()
endif()