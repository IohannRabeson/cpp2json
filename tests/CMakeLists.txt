find_package(Boost REQUIRED COMPONENTS system unit_test_framework filesystem)

include_directories(include ${Boost_INCLUDE_DIR})

set(INC_DIR include)
set(SRC_DIR src)

set(COMMON_SRC
    ${SRC_DIR}/main.cpp
)

# Put here your hpp and cpp of tests
set(SRC_TEST
    ${SRC_DIR}/TestCpp2Json.cpp
)

set(LIBRARIES ${Boost_LIBRARIES})
set(OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/tests")
set(INCLUDE_DIRS ${INC_DIR} ${RAPIDJSON_INCLUDE_DIRS})
add_definitions(-DTEST_DATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}" -DEXECUTABLE_PATH="${CMAKE_BINARY_DIR}/output/cpp2json")
create_tests(COMMON_SRC SRC_TEST INCLUDE_DIRS LIBRARIES ${OUTPUT_DIRECTORY})
