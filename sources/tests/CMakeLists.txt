set(GTEST_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/dependencies/googletest/googletest/include/")

set(SRC
    Cpp2JsonVisitorFixture.hpp
    Cpp2JsonVisitorFixture.cpp
    Cpp2JsonVisitorTest.cpp

    ../JsonOutputWriter.hpp
    ../JsonOutputWriter.cpp
    gtest.cpp
)


add_executable(unit_tests ${SRC})
set_target_properties(unit_tests PROPERTIES COMPILE_FLAGS -fvisibility-inlines-hidden)
target_compile_definitions(unit_tests PRIVATE LLVM_DIR="${LLVM_DIR}" RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources")
target_link_libraries(unit_tests PRIVATE cpp2json-core gtest gmock)
target_include_directories(unit_tests PRIVATE "${GTEST_INCLUDE_DIR}")
set_property(TARGET unit_tests PROPERTY CXX_STANDARD 14)
set_property(TARGET unit_tests PROPERTY CXX_STANDARD_REQUIRED ON)
