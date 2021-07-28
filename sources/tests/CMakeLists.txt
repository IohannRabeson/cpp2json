set(GTEST_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/dependencies/googletest/googletest/include/")

set(SRC
    Cpp2JsonVisitorFixture.hpp
    Cpp2JsonVisitorFixture.cpp
    Cpp2JsonVisitorTest.cpp

    ../app/JsonOutputWriter.hpp
    ../app/JsonOutputWriter.cpp
    gtest.cpp
    resources/test_example.hpp
)

add_executable(unit_tests ${SRC})
set_target_properties(unit_tests PROPERTIES COMPILE_FLAGS -fvisibility-inlines-hidden)
target_compile_definitions(unit_tests PRIVATE LLVM_DIR="${LLVM_DIR}"
                                              CPP2JSON_SHARED_HEADER="-I${CMAKE_SOURCE_DIR}/sources/shared/"
                                              RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources"
)
target_link_libraries(unit_tests PRIVATE cpp2json-core cpp2json-shared gtest gmock)
target_include_directories(unit_tests PRIVATE "${GTEST_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}/sources/app")
set_property(TARGET unit_tests PROPERTY CXX_STANDARD 14)
set_property(TARGET unit_tests PROPERTY CXX_STANDARD_REQUIRED ON)
cpp2json_private_stuff(unit_tests PRIVATE)

include(GoogleTest)
gtest_discover_tests(unit_tests)
