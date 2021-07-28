find_package(Git REQUIRED)

macro(gather_git_infos)
    exec_program(${GIT_EXECUTABLE} "${CMAKE_SOURCE_DIR}" ARGS "describe --abbrev=0 --tags --always" OUTPUT_VARIABLE GIT_TAG ERROR_QUIET)
    exec_program(${GIT_EXECUTABLE} "${CMAKE_SOURCE_DIR}" ARGS "rev-parse --abbrev-ref HEAD" OUTPUT_VARIABLE GIT_BRANCH)
    exec_program(${GIT_EXECUTABLE} "${CMAKE_SOURCE_DIR}" ARGS "rev-parse --short HEAD" OUTPUT_VARIABLE GIT_SHORT_COMMIT)
    string(STRIP "${GIT_TAG}" GIT_TAG)
    string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
    string(STRIP "${GIT_SHORT_COMMIT}" GIT_SHORT_COMMIT)
endmacro(gather_git_infos)

execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        RESULT_VARIABLE GIT_SUBMOD_RESULT)

if(NOT GIT_SUBMOD_RESULT EQUAL "0")
    message(STATUS "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
endif()

gather_git_infos()
