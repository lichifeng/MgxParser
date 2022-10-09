cmake_minimum_required(VERSION 3.0.0)

# Project info
project(
    MgxParser
    VERSION 0.1.0 
    LANGUAGES CXX 
    HOMEPAGE_URL "aocrec.com" 
    DESCRIPTION "A C++ program used to extract useful information from Age of Empires II game records."
)

# Some basic configurations
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXE_LINKER_FLAGS "-static")

# List source files to be compiled
aux_source_directory(./src SRC_ROOT)
aux_source_directory(./src/DataModels DATAMODELS)
aux_source_directory(./src/Analyzers/default DEFAULT_ANALYZER)
aux_source_directory(./src/Analyzers/default/bodyProcessors DEFAULT_ANALYZER_BODY)
add_executable(${PROJECT_NAME} ${SRC_ROOT} ${DEFAULT_ANALYZER} ${DEFAULT_ANALYZER_BODY} ${DATAMODELS})

# Import dependencies
# WARNING: png depends on libz, so put it before z
target_link_libraries(${PROJECT_NAME} pthread)
target_link_libraries(${PROJECT_NAME} png)
target_link_libraries(${PROJECT_NAME} z)
target_link_libraries(${PROJECT_NAME} "stdc++fs")

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/libs/")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/")
target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_BINARY_DIR}")

# Import some environment variables and generate docs, etc.
string(TIMESTAMP COMPILEDATE "%Y%m%d")
target_compile_definitions(${PROJECT_NAME} PRIVATE "DEBUG=$<CONFIG:Debug>")
configure_file(CompileConfig.h.in CompileConfig.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/README.md ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
# Update doxygen configuration file and generate output HTML
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})
if(BUILD_DOCUMENTATION)
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