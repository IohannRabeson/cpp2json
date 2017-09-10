add_library(cpp2json_shared INTERFACE)
target_sources(cpp2json_shared INTERFACE Cpp2JsonAnnotations.hpp)

target_include_directories(cpp2json_shared INTERFACE "${PROJECT_SOURCE_DIR}/shared")

target_include_directories(cpp2json_shared INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/shared>
    $<INSTALL_INTERFACE:${YOUR_INSTALL_DIR}/shared>
)
