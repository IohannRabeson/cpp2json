cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(cpp2json)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

find_package(RapidJSON 1.1 REQUIRED)
find_package(LLVM 5 REQUIRED)
find_package(Clang REQUIRED)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CreateOption.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/GitInfo.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/PrependList.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CreateTest.cmake")

add_subdirectory(sources)

