macro(create_tests COMMON_SOURCE_FILES SOURCE_FILES INCLUDE_DIRS LIBRARIES OUTPUT_DIRECTORY)
    foreach(SRC_FILE ${${SOURCE_FILES}})
        #Extract the filename without extension (NAME_WE)
        get_filename_component(TEST_NAME ${SRC_FILE} NAME_WE)

        add_executable(${TEST_NAME} ${SRC_FILE} ${${COMMON_SOURCE_FILES}})
        set_property(TARGET ${TEST_NAME} PROPERTY CXX_STANDARD 14)
        set_property(TARGET ${TEST_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

        #link to Boost libraries and other targets and dependencies
        include_directories(${${INCLUDE_DIRS}})
        target_link_libraries(${TEST_NAME} ${${LIBRARIES}})
        set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}")

        #Finally add it to test execution
        #Notice the WORKING_DIRECTORY and COMMAND
        add_test(NAME ${TEST_NAME}
            WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
            COMMAND "$<TARGET_FILE:${TEST_NAME}>"
            )
    endforeach(SRC_FILE)
endmacro(create_tests)
