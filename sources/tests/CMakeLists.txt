find_package(GTest)

set(GTEST_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/dependencies/googletest/googletest/include/")
set(SRC
    gtest.cpp
)

gtest_add_tests(unit_tests "" ${SRC})
add_executable(unit_tests ${SRC})
target_link_libraries(unit_tests PRIVATE gtest gmock)
target_include_directories(unit_tests PRIVATE "${GTEST_INCLUDE_DIR}")
set_property(TARGET unit_tests PROPERTY CXX_STANDARD 14)
set_property(TARGET unit_tests PROPERTY CXX_STANDARD_REQUIRED ON)
