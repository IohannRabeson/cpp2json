set(SOURCES
    JsonOutputWriter.hpp
    JsonOutputWriter.cpp
    main.cpp
    ${PROJECT_SOURCE_DIR}/README.md
)

add_executable(cpp2json ${SOURCES})

set_target_properties(cpp2json PROPERTIES COMPILE_FLAGS -fvisibility-inlines-hidden)
target_link_libraries(cpp2json PRIVATE cpp2json-core)
target_compile_definitions(cpp2json PRIVATE __STDC_CONSTANT_MACROS __STDC_LIMIT_MACROS RAPIDJSON_HAS_STDSTRING=1)
set_property(TARGET cpp2json PROPERTY CXX_STANDARD 14)
set_property(TARGET cpp2json PROPERTY CXX_STANDARD_REQUIRED ON)
