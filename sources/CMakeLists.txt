find_package(LLVM REQUIRED)
find_package(Clang REQUIRED)
find_package(RapidJSON REQUIRED)

set(SOURCES
    main.cpp
    ../README.md
)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

add_executable(cpp2json ${SOURCES})

add_library(cpp2json-core
            Cpp2JsonVisitor.hpp
            Cpp2JsonVisitor.cpp
            Cpp2JsonAction.hpp
            Cpp2JsonAction.cpp
            Cpp2JsonActionFactory.hpp
            Cpp2JsonActionFactory.cpp
            Cpp2JsonParameters.hpp
            JsonOutputWriter.hpp
            JsonOutputWriter.cpp)

get_filename_component(LLVM_DIR ${LLVM_INCLUDE_DIR} DIRECTORY)

target_link_libraries(cpp2json PRIVATE cpp2json-core)
target_compile_definitions(cpp2json PRIVATE LLVM_DIR="${LLVM_DIR}")
target_compile_definitions(cpp2json PRIVATE __STDC_CONSTANT_MACROS __STDC_LIMIT_MACROS RAPIDJSON_HAS_STDSTRING=1)

set_target_properties(cpp2json-core PROPERTIES COMPILE_FLAGS ${LLVM_COMPILE_FLAGS})
target_link_libraries(cpp2json-core PRIVATE ${LLVM_LIBS_CORE} ${CLANG_LIBS} ${Boost_LIBRARIES} ${LLVM_LDFLAGS})
target_include_directories(cpp2json-core PUBLIC "${LLVM_INCLUDE_DIR}" "${CLANG_INCLUDE_DIRS}" "${Boost_INCLUDE_DIRS}" "${RAPIDJSON_INCLUDE_DIRS}")
target_compile_definitions(cpp2json-core PRIVATE __STDC_CONSTANT_MACROS __STDC_LIMIT_MACROS RAPIDJSON_HAS_STDSTRING=1)

set_property(TARGET cpp2json PROPERTY CXX_STANDARD 14)
set_property(TARGET cpp2json PROPERTY CXX_STANDARD_REQUIRED ON)

