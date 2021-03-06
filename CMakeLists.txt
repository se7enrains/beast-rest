cmake_minimum_required(VERSION 3.15)
project(simple_rest_service)

set(CMAKE_CXX_STANDARD 14)


find_package(Boost COMPONENTS system coroutine filesystem REQUIRED)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/rapidjson/CMakeLists.txt"
        OR NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/libconfig/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

FIND_LIBRARY(LIBCONFIGPP_LIBRARY config++
        ${PROJECT_SOURCE_DIR}
        )

add_subdirectory(server)

add_subdirectory(client)