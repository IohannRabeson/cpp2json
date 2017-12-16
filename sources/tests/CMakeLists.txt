set(GTEST_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/dependencies/googletest/googletest/include/")

set(SRC
    gtest.cpp
)

add_executable(unit_tests ${SRC})
target_link_libraries(unit_tests PRIVATE cpp2json-core gtest gmock)
target_include_directories(unit_tests PRIVATE "${GTEST_INCLUDE_DIR}")
set_property(TARGET unit_tests PROPERTY CXX_STANDARD 14)
set_property(TARGET unit_tests PROPERTY CXX_STANDARD_REQUIRED ON)
