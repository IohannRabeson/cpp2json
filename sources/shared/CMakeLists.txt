add_library(cpp2json-shared INTERFACE)
cpp2json_private_stuff(cpp2json-shared)
target_include_directories(cpp2json-shared INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
