cmake_minimum_required(VERSION 3.15)
project(Lab1Var1 C)

set(CMAKE_C_STANDARD 99)

add_executable(Lab1Var1 main.c)

add_library(supp SHARED supplementary/trialfuncs.c)

if(MSVC)
    include(GenerateExportHeader)
    generate_export_header(supp
            BASE_NAME supp
            EXPORT_MACRO_NAME LAB1_EXPORTS
            EXPORT_FILE_NAME trialfuncs_exports.h
            STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)
endif()

target_include_directories(supp PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/include"
        )

target_include_directories(Lab1Var1 PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/include"
        )

target_link_libraries(Lab1Var1 PUBLIC supp)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")