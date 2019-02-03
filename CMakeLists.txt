cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(cpp2json)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

find_package(RapidJSON 1.1 REQUIRED)
find_package(LLVM 6 REQUIRED)
find_package(Clang REQUIRED)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CreateOption.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/GitInfo.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/PrependList.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CreateTest.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Cpp2JsonPrivateDefinition.cmake")

add_subdirectory(external)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

if (NOT CPP2JSON_VERSION)
    set(CPP2JSON_VERSION "${GIT_BRANCH}-${GIT_SHORT_COMMIT}")
    if (TRAVIS_BUILD_NUMBER)
        set(CPP2JSON_VERSION "${CPP2JSON_VERSION}-${TRAVIS_BUILD_NUMBER}")
    endif()
endif()

message("Build version: ${CPP2JSON_VERSION}")

add_subdirectory(sources)

