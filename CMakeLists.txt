cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(cpp2json)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(RapidJSON 1.1 REQUIRED)
find_package(LLVM 4.0.1 REQUIRED)
find_package(Clang REQUIRED)

include("${CMAKE_SOURCE_DIR}/cmake/CreateOption.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/GitInfo.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/PrependList.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/CreateTest.cmake")

add_subdirectory(sources)
add_subdirectory(shared)
add_subdirectory(tests)

