CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
project(server)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost COMPONENTS system coroutine REQUIRED)
include_directories(include ${Boost_INCLUDE_DIRS})
add_executable(server
        src/main.cpp
        src/server.cpp
        src/httphandler.cpp)

target_link_libraries(server
        ${Boost_LIBRARIES})
