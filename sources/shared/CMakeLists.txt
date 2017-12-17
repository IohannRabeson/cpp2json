add_library(cpp2json-shared INTERFACE)
target_include_directories(cpp2json-shared INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(cpp2json-shared INTERFACE
                           CPP2JSON_EXCLUDE_ANNOTATION_TAG="cpp2json_exclude"

)
