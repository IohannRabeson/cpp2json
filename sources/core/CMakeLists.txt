set(BACKEND_SOURCES
    include/Cpp2JsonVisitor.hpp
    src/Cpp2JsonVisitor.cpp
    include/Cpp2JsonAction.hpp
    src/Cpp2JsonAction.cpp
    include/Cpp2JsonActionFactory.hpp
    src/Cpp2JsonActionFactory.cpp
    include/Cpp2JsonParameters.hpp
    include/CommandLineHelpers.hpp
    src/CommandLineHelpers.cpp
)

get_filename_component(LLVM_DIR ${LLVM_INCLUDE_DIR} DIRECTORY)

add_library(cpp2json-core ${BACKEND_SOURCES})
set_target_properties(cpp2json-core PROPERTIES COMPILE_FLAGS -fvisibility-inlines-hidden)
target_link_libraries(cpp2json-core PUBLIC ${LLVM_LIBS_CORE} ${CLANG_LIBS} ${Boost_LIBRARIES} ${LLVM_LDFLAGS})
target_include_directories(cpp2json-core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/sources/shared" "${LLVM_INCLUDE_DIR}" "${CLANG_INCLUDE_DIRS}" "${Boost_INCLUDE_DIRS}" "${RAPIDJSON_INCLUDE_DIRS}")
target_compile_definitions(cpp2json-core PRIVATE LLVM_DIR="${LLVM_DIR}")
target_compile_definitions(cpp2json-core PRIVATE __STDC_CONSTANT_MACROS __STDC_LIMIT_MACROS RAPIDJSON_HAS_STDSTRING=1)
target_compile_definitions(cpp2json-core PRIVATE CPP2JSON_CORE_VERSION="${CPP2JSON_CORE_VERSION}")
set_property(TARGET cpp2json-core PROPERTY CXX_STANDARD 14)
set_property(TARGET cpp2json-core PROPERTY CXX_STANDARD_REQUIRED ON)
cpp2json_private_stuff(cpp2json-core PRIVATE)

target_include_directories(cpp2json-core PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)
