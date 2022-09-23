cmake_minimum_required(VERSION 3.0.0)
project(
    MgxParser
    VERSION 0.1.0 
    DESCRIPTION "A C++ program used to extract useful information from Age of Empires II game records."
    HOMEPAGE_URL "aocrec.com"
)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

aux_source_directory(./src SRC_ROOT)
aux_source_directory(./src/Analyzers/default DEFAULT_ANALYZER)
add_executable(mgxparser ${SRC_ROOT} ${DEFAULT_ANALYZER})

target_link_libraries(mgxparser z)
target_link_libraries(mgxparser png)
target_link_libraries(mgxparser pthread)

configure_file(CompileConfig.h.in CompileConfig.h)

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

target_include_directories(mgxparser PUBLIC "${PROJECT_BINARY_DIR}")